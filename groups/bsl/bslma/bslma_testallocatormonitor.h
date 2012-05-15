// bslma_testallocatormonitor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_TESTALLOCATORMONITOR
#define INCLUDED_BSLMA_TESTALLOCATORMONITOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to summarize 'bslma::TestAllocator' object use.
//
//@CLASSES:
//  bslma::TestAllocatorMonitor: 'bslma::TestAllocator' summary mechanism
//
//@AUTHOR: Rohan Bhindwale (rbhindwale), Steven Breitstein (sbreitstein)
//
//@SEE_ALSO: bslma_testallocator
//
//@DESCRIPTION: This component provides a single mechanism class,
// 'bslma::TestAllocatorMonitor', which is used, in concert with
// 'bslma::TestAllocator', in the implementation of test drivers.  The
// 'bslma::TestAllocatorMonitor' class provides boolean accessors indicating
// whether associated test allocator state has changed (or not) since
// construction of the monitor.  Using 'bslma::TestAllocatorMonitor' objects
// often result in test cases that are more concise, easier to read, and less
// error prone than test cases that directly access the test allocator for
// state information.
//
///Statistics
///----------
// The test allocator statistics tracked by the test allocator monitor along
// with the boolean accessors used to observe a change in those statistics are
// shown in the table below.  The change (or lack of change) reported by these
// accessors are relative to the value of the test allocator statistic at the
// construction of the monitor.  Note that each of these statistics count
// blocks of memory (i.e., number of allocations from the allocator), and do
// not depend on the number of bytes in those allocated blocks.
//..
//  Statistic        Is-Same Method Is-Up Method Is-Down Method
//  --------------   -------------- ------------ --------------
//  numBlocksInUse   isInUseSame    isInUseUp    isInUseDown
//  numBlocksMax     isMaxSame      isMaxUp      none
//  numBlocksTotal   isTotalSame    isTotalUp    none
//..
// The 'numBlocksMax' and 'numBlocksTotal' statistics have values that are
// monotonically non-decreasing; hence, they need no "Is-Down" methods.  Note
// that if a monitor is created for an allocator with outstanding blocks ("in
// use"), then it is possible for the allocator's count of outstanding blocks
// to drop below the value seen by the monitor at construction.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Standard Usage
///- - - - - - - - - - - - -
// Classes taking 'bslma_allocator' objects have many requirements (and thus,
// many testing concerns) that other classes do not.  Here we illustrate how
// 'bslma::TestAllocatorMonitor' objects (in conjunction with
// 'bslma::TestAllocator' objects) can be used in a test driver to succinctly
// address many concerns of an object's use of allocators.
//
// First, for a test subject, we introduce 'MyClass', an unconstrained
// attribute class having a single, null-terminated ascii string attribute,
// 'description'.  For the sake of brevity, 'MyClass' defines only a default
// constructor, a primary manipulator (the 'setDescription' method), and a
// basic accessor (the 'description' method).  These suffice for the purposes
// of these example.  Note that a proper attribute class would also implement
// value and copy constructors, 'operator==', an accessor for the allocator,
// and other methods.
//..
//  class MyClass {
//      // This unconstrained (value-semantic) attribute class has a single,
//      // null-terminated ascii string attribute, 'description'.
//
//      // DATA
//      int               d_capacity;      // available memory
//      char             *d_description_p; // string data
//      bslma::Allocator *d_allocator_p;   // held, not owned
//
//    public:
//      // CREATORS
//      explicit MyClass(bslma::Allocator *basicAllocator = 0);
//          // Create a 'MyClass' object having the (default) attribute values:
//          //..
//          //  description() == ""
//          //..
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      ~MyClass();
//          // Destroy this object.
//
//      // MANIPULATORS
//      void setDescription(const char *value);
//          // Set the null-terminated ascii string 'description' attribute of
//          // this object to the specified 'value'.  On completion, the
//          // 'description' method returns the address of a copy of the ascii
//          // string at 'value'.
//
//      // ACCESSORS
//      const char *description() const;
//          // Return the value of the null-terminated ascii string
//          // 'description' attribute of this object.
//  };
//
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ========================================================================
//
//                          // -------------
//                          // class MyClass
//                          // -------------
//
//  // CREATORS
//  inline
//  MyClass::MyClass(bslma::Allocator *basicAllocator)
//  : d_capacity(0)
//  , d_description_p(0)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  inline
//  MyClass::~MyClass()
//  {
//      BSLS_ASSERT_SAFE(0 <= d_capacity);
//
//      d_allocator_p->deallocate(d_description_p);
//  }
//
//  // MANIPULATORS
//  inline
//  void MyClass::setDescription(const char *value)
//  {
//      BSLS_ASSERT_SAFE(value);
//
//      int size = std::strlen(value) + 1;
//      if (size > d_capacity) {
//          char *newMemory = (char *) d_allocator_p->allocate(size);
//          d_allocator_p->deallocate(d_description_p);
//          d_description_p = newMemory;
//          d_capacity      = size;
//
//      }
//      std::memcpy(d_description_p, value, size);
//  }
//..
// Notice that the implementation of the manipulator allocates/deallocates
// memory *before* updating the object.  This ordering leaves the object
// unchanged in case the allocator throws an exception (part of the strong
// exception guarantee).  This is an implementation detail, not a part of the
// contract (in this example).
//..
//
//  // ACCESSORS
//  inline
//  const char *MyClass::description() const
//  {
//      return d_description_p ? d_description_p : "";
//  }
//..
// Then, we design a test-driver for 'MyClass'.  Our allocator-related concerns
// for 'MyClass' include:
//..
// Concerns:
//: 1 Any memory allocation is from the object allocator.
//:
//: 2 Every object releases any allocated memory at destruction.
//:
//: 3 No accessor allocates any memory.
//:
//: 4 All memory allocation is exception neutral.
//:
//: 5 QoI: The default constructor allocates no memory.
//:
//: 6 QoI: When possible, memory is cached for reuse.
//..
// Notice that some of these concerns (e.g., C-5..6) are not part of the
// class's documented, contractual behavior.  These are classified as Quality
// of Implementation (QoI) concerns.
//
// Next, we define a test plan.  For example, a plan to test these concerns is:
//..
// Plan:
//: 1 Setup global and default allocators:
//:
//:   1 Create two 'bslma::TestAllocator' objects and, for each of these,
//:     create an associated 'bslma::TestAllocatorMonitor' object.
//:
//:   2 Install the two allocators as the global and default allocators.
//:
//: 2 Confirm that default construction allocates no memory: (C-5)
//:
//:   1 Construct a 'bslma::TestAllocatorMonitor' object to be used passed to
//:     test objects on their construction, and an associated
//:
//:   2 In an inner block, default construct an object of 'MyClass' using the
//:     designated "object" test allocator.
//:
//:   3 Allow the object to go out of scope (destroyed).  Confirm that no
//:     memory has been allocated from any of the allocators.
//:
//: 3 Exercise an object of 'MyClass' such that memory should be allocated, and
//:   then confirm that the object allocator (only) is used: (C-2..4,6)
//:
//:   1 In another inner block, default construct a new test object using the
//:     (as yet unused) object allocator.
//:
//:   2 Force the test object to allocate memory by setting its 'descriptor'
//:     attribute to a value whose size exceeds the size of the object itself.
//:     Confirm that the attribute was set and that memory was allocated.
//:
//:   3 Confirm the the primary manipulator (the 'setDescription' method) is
//:     exception neutral (i.e., exceptions from the allocator are propagated
//:     and no memory is leaked).  Use the
//:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros to manage the test,
//:     and use the test allocator monitor to confirm that memory is allocated
//:     on the no-exception code path.  (C-4)
//:
//:   4 When the object is holding memory, create an additional test allocator
//:     monitor allocator for the object allocator.  Use the basic accessor
//:     (i.e., the 'description' method) to confirm that the object has the
//:     expected value.  Check this test allocator monitor to confirm that
//:     accessor allocated no memory.  (C-3)
//:
//:   5 Change the attribute to a smaller value and confirm that the current
//:     memory was reused (i.e., no memory is allocated). (C-6)
//:
//:   6 Destroy the test object by allowing it to go out of scope, and confirm
//:     that all allocations are returned.  (C-2)
//:
//: 4 Confirm that at no time were the global allocator or the default
//:   allocator were used.  (C-1)
//..
// The implementation of the plan is shown below:
//
// Then, we implement the first portion of the plan.  We create the trio of
// test allocators, their respective test allocator monitors, and install two
// of the allocators as the global and default allocators:
//..
//  {
//      if (verbose) cout << "Setup global and default allocators" << endl;
//
//      bslma::TestAllocator        ga("global",  veryVeryVeryVerbose);
//      bslma::TestAllocator        da("default", veryVeryVeryVerbose);
//      bslma::TestAllocatorMonitor gam(&ga);
//      bslma::TestAllocatorMonitor dam(&da);
//
//      bslma::Default::setGlobalAllocator(&ga);
//      bslma::Default::setDefaultAllocatorRaw(&da);
//..
// Then, we default construct a test object using the object allocator, and
// then, immediately destroy it.  The object allocator monitor, 'oam', shows
// that the allocator was not used.
//..
//  if (verbose) cout << "No allocation by Default Constructor " << endl;
//
//  bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
//  bslma::TestAllocatorMonitor oam(&oa);
//
//  {
//      MyClass obj(&oa);
//      assert(oam.isTotalSame()); // object allocator unused
//  }
//..
// Next, we pass the (still unused) object allocator to another test object.
// This time, we coerce the object into allocating memory by setting an
// attribute.  (Setting an attribute larger than the receiving object means
// that the object cannot store the data within its own footprint and must
// allocate memory.)
//..
//  if (verbose) cout << "Exercise object" << endl;
//
//  {
//      MyClass obj(&oa);
//
//      const char DESCRIPTION1[]="abcdefghijklmnopqrstuvwyz"
//                                "abcdefghijklmnopqrstuvwyz";
//      assert(sizeof(obj) < sizeof(DESCRIPTION1));
//
//      if (veryVerbose) cout << "\tPrimary Manipulator Allocates" << endl;
//
//      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
//          if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }
//
//          obj.setDescription(DESCRIPTION1);
//          assert(oam.isTotalUp());  // object allocator was used
//          assert(oam.isInUseUp());  // some outstanding allocation(s)
//          assert(oam.isMaxUp());    // a maximum was set
//      } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
//..
// Notice, as expected, memory was allocated from object allocator.
//
// Now that the allocator has been used, we create a second monitor to capture
// the that state.  Confirm that the basic accessor (the 'description' method)
// does not use the allocator.
//..
//  if (veryVerbose) cout << "\tBasic Accessor does not allocate"
//                        << endl;
//
//  bslma::TestAllocatorMonitor oam2(&oa); // Captures state of 'oa'
//                                        // with outstanding
//                                        // allocations.
//
//  assert(0 == strcmp(DESCRIPTION1, obj.description()));
//  assert(oam2.isTotalSame());  // object allocator was not used
//..
// Next, confirm that when a shorter value is assigned, the existing memory is
// reused.
//..
//
//  obj.setDescription("a");
//  assert(0 == std::strcmp("a", obj.description()));
//
//  assert(oam2.isTotalSame());  // no allocations
//..
// Notice that there are no allocations because the object had sufficient
// capacity in previously allocated memory to store the short string.
//
// Next, as an additional test, we make the object allocate additional memory
// by setting a longer attribute: one that exceeds the capacity allocated for
// 'DESCRIPTION1'.  Use the second monitor to confirm that an allocation was
// performed.
//
// There are tests where using a test allocator monitor does not suffice.  Our
// test object is currently holding memory, if we assign a value that exceeds
// its current capacity there will be two operations on the object allocator:
// the allocation of larger memory, and the deallocation of its current memory:
// in that order, as part of the strong exception guarantee.  Thus, the maximum
// number of allocations should go up by one, and no more.
//
// Note that absence of memory leaks due to exceptions (the other part of the
// strong exception guarantee is confirmed during the destruction of the object
// test allocator at the end of this test, which featured exceptions.
//..
//  bsls::Types::Int64 maxBeforeSet   = oa.numBlocksMax();
//  const char        DESCRIPTION2[] = "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz"
//                                     "abcdefghijklmnopqrstuvwyz";
//  assert(sizeof(DESCRIPTION1) < sizeof(DESCRIPTION2));
//
//  obj.setDescription(DESCRIPTION2);
//  assert(0 == std::strcmp(DESCRIPTION2, obj.description()));
//
//  assert(oam2.isTotalUp());    // The object allocator used.
//
//  assert(oam2.isInUseSame());  // The outstanding block (allocation)
//                               // count unchanged (even though byte
//                               // outstanding byte count increased).
//
//  assert(oam2.isMaxUp());      // Max increased as expected, but was
//                               // did it change only by one?  The
//                               // monitor cannot answer that
//                               // question.
//
//  bsls::Types::Int64 maxAfterSet = oa.numBlocksMax();
//
//  assert(1 == maxAfterSet - maxBeforeSet);
//..
// Notice that our test allocator monitor cannot confirm that the allocator's
// maximum increased by exactly one.  In this case, we must extract our
// statistics directly from the test allocator.
//
// Note that increment in "max" occurs only the first time through the the
// allocate/deallocate scenario in 'setDescription'.
//..
//  bslma::TestAllocatorMonitor oam3(&oa);
//
//  const char DESCRIPTION3[] = "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz"
//                              "abcdefghijklmnopqrstuvwyz";
//  assert(sizeof(DESCRIPTION2) < sizeof(DESCRIPTION3));
//
//  obj.setDescription(DESCRIPTION3);
//  assert(0 == std::strcmp(DESCRIPTION3, obj.description()));
//
//  assert(oam3.isTotalUp());    // The object allocator used.
//
//  assert(oam3.isInUseSame());  // The outstanding block (allocation)
//                               // count unchanged (even though byte
//                               // outstanding byte count increased).
//
//  assert(oam3.isMaxSame());    // A repeat of the scenario for
//                               // 'DESCRIPTION2', so no change in the
//                               // allocator's maximum.
//..
// Now, we close scope and check that all object memory was deallocated
//..
//  }
//
//  if (veryVerbose) cout << "\tAll memory returned object allocator"
//                        << endl;
//
//  assert(oam.isInUseSame());
//..
// Finally, we check that none of these operations used the default or global
// allocators.
//..
//      if (verbose) cout << "Global and Default allocators never used"
//                        << endl;
//
//      assert(gam.isTotalSame());
//      assert(dam.isTotalSame());
//  }
///Example 2: The 'reset' method
///- - - - - - - - - - - - - - -
// To demonstrate the usefulness of the 'reset' method, one needs to be testing
// a class having multiple manipulators that allocate memory.  For brevity we
// use 'bsl::list', whose interface is clearly described at
// 'http://www.cplusplus.com/reference/stl/list' except that a 'bsl::list' uses
// memory from a memory allocator supplied at construction.
//..
//  typedef bsl::list<int> IntList;
//..
// In this example, we observe how some of the manipulators and
// accessors of 'bsl::list' use memory.
//
// First, we declare 3 allocators, Test Object Allocator ('toa'), Test
// Default Allocator ('tda'), and Test Global Allocator ('tga'):
//..
//  bslma_TestAllocator toa;
//  bslma_TestAllocator tda;
//  bslma_TestAllocator tga;
//..
// Since neither the default nor the global allocators should ever be
// have used for anything in this exercise, we only need check their
// monitors 'dam' and 'gam' at the end of the example.
//
// Then, we install our default and global allocators:
//..
//  bslma_DefaultAllocatorGuard defaultGuard(&tda);
//  bslma_Default::setGlobalAllocator(&tga);
//..
// Next, we set up our monitors of the 3 allocators:
//..
//  bslma_TestAllocatorMonitor oam(&toa);
//  bslma_TestAllocatorMonitor dam(&tda);
//  bslma_TestAllocatorMonitor gam(&tga);
//..
// Then, we observe that creating an empty list allocates memory:
//..
//  IntList myList(&toa);
//
//  assert(oam.isTotalUp());
//..
// Next, we observe that 'push_back' also allocates memory:
//..
//  oam.reset();
//
//  myList.push_back(57);
//
//  assert(oam.isTotalUp());
//
//  myList.push_back(57);
//..
// Then, we observe that accessors 'back', 'front', and 'size' don't
// cause any allocation.
//..
//  oam.reset();
//
//  assert(57 == myList.back());
//  assert(57 == myList.front());
//  assert(2  == myList.size());
//
//  assert(oam.isTotalSame());
//..
// Next, we observe that manipulator 'push_front' causes memory
// allocation:
//..
//  myList.push_front(23);
//
//  assert(oam.isTotalUp());
//
//  assert(57 == myList.back());
//  assert(23 == myList.front());
//  assert(3  == myList.size());
//..
// Then, we observe that 'reverse', though it's a manipulator, does no
// memory allocation:
//..
//  oam.reset();
//
//  myList.reverse();
//
//  assert(oam.isInUseSame());
//  assert(oam.isTotalSame());
//
//  assert(23 == myList.back());
//  assert(57 == myList.front());
//  assert(3  == myList.size());
//..
// Next, we observe that 'unique', in this case, will do no new memory
// allocation and will in fact free some memory.
//..
//  oam.reset();
//
//  myList.unique();
//
//  assert(oam.isInUseDown());
//  assert(oam.isTotalSame());
//..
// Now, we observe the state of the list after 'unique'.
//..
//  assert(23 == myList.back());
//  assert(57 == myList.front());
//  assert(2  == myList.size());
//..
// Finally, we check the default and global allocator monitors.
//..
//  assert(dam.isTotalSame());
//  assert(gam.isTotalSame());
//..

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ==========================
                        // class TestAllocatorMonitor
                        // ==========================

class TestAllocatorMonitor {
    // This mechanism provides a set of boolean accessor methods indicating
    // whether a change has occurred in the state of the 'TestAllocator' object
    // (supplied at construction) since the construction of the monitor.  See
    // the Statistics section of @DESCRIPTION for the statics tracked.

    // DATA
    const bsls::Types::Int64          d_initialInUse;    // 'numBlocksInUse'
    const bsls::Types::Int64          d_initialMax;      // 'numBlocksMax'
    const bsls::Types::Int64          d_initialTotal;    // 'numBlocksTotal'
    const TestAllocator *const d_testAllocator_p; // held, not owned

    static const TestAllocator *validateArgument(
                                               const TestAllocator *allocator);
        // Return the specified 'allocator', and, if compiled in "SAFE" mode,
        // assert that 'allocator' is not 0.  Note that this static function is
        // needed to perform validation on the allocator address supplied at
        // construction, prior to that address being dereferenced to initialize
        // the 'const' data members of this type.

  public:
    // CREATORS
    explicit TestAllocatorMonitor(const TestAllocator *testAllocator);
        // Create a 'TestAllocatorMonitor' object to track changes in
        // statistics of the specified 'testAllocator'.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~TestAllocatorMonitor();
        // Destroy this object.
#endif

    // ACCESSORS
    bool isInUseDown() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has decreased since construction of this monitor, and
        // 'false' otherwise.

    bool isInUseSame() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isInUseUp() const;
        // Return 'true' if the 'numBlocksInUse' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bool isMaxSame() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isMaxUp() const;
        // Return 'true' if the 'numBlocksMax' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bool isTotalSame() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked test
        // allocator has not changed since construction of this monitor, and
        // 'false' otherwise.

    bool isTotalUp() const;
        // Return 'true' if the 'numBlocksTotal' statistic of the tracked test
        // allocator has increased since construction of this monitor, and
        // 'false' otherwise.

    bsls::Types::Int64 numBlocksInUseChange() const;
        // Return the change in the 'numBlocksInUse' statistic of the tracked
        // test allocator since construction of this monitor.

    bsls::Types::Int64 numBlocksMaxChange() const;
        // Return the change in the 'numBlocksMax' statistic of the tracked
        // test allocator since construction of this monitor.

    bsls::Types::Int64 numBlocksTotalChange() const;
        // Return the change in the 'numBlocksTotal' statistic of the tracked
        // test allocator since construction of this monitor.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class TestAllocatorMonitor
                        // --------------------------

// CLASS METHODS
inline
const TestAllocator *
TestAllocatorMonitor::validateArgument(const TestAllocator *allocator)
{
    BSLS_ASSERT_SAFE(allocator);

    return allocator;
}

// CREATORS
inline
TestAllocatorMonitor::TestAllocatorMonitor(const TestAllocator *testAllocator)
: d_initialInUse((validateArgument(testAllocator))->numBlocksInUse())
, d_initialMax(testAllocator->numBlocksMax())
, d_initialTotal(testAllocator->numBlocksTotal())
, d_testAllocator_p(testAllocator)
{
}

}  // close package namespace

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

namespace bslma {

inline
TestAllocatorMonitor::~TestAllocatorMonitor()
{
    BSLS_ASSERT_SAFE(d_testAllocator_p);
    BSLS_ASSERT_SAFE(0 <= d_initialMax);
    BSLS_ASSERT_SAFE(0 <= d_initialTotal);
}

}  // close package namespace

#endif

namespace bslma {

// ACCESSORS
inline
bool TestAllocatorMonitor::isInUseDown() const
{
    return d_testAllocator_p->numBlocksInUse() < d_initialInUse;
}

inline
bool TestAllocatorMonitor::isInUseSame() const
{
    return d_testAllocator_p->numBlocksInUse() == d_initialInUse;
}

inline
bool TestAllocatorMonitor::isInUseUp() const
{
    return d_testAllocator_p->numBlocksInUse() > d_initialInUse;
}

inline
bool TestAllocatorMonitor::isMaxSame() const
{
    return d_initialMax == d_testAllocator_p->numBlocksMax();
}

inline
bool TestAllocatorMonitor::isMaxUp() const
{
    return d_testAllocator_p->numBlocksMax() != d_initialMax;
}

inline
bool TestAllocatorMonitor::isTotalSame() const
{
    return d_testAllocator_p->numBlocksTotal() == d_initialTotal;
}

inline
bool TestAllocatorMonitor::isTotalUp() const
{
    return d_testAllocator_p->numBlocksTotal() != d_initialTotal;
}

inline
bsls::Types::Int64 TestAllocatorMonitor::numBlocksInUseChange() const
{
    return d_testAllocator_p->numBlocksInUse() - d_initialInUse;
}

inline
bsls::Types::Int64 TestAllocatorMonitor::numBlocksMaxChange() const
{
    return d_testAllocator_p->numBlocksMax() - d_initialMax;
}

inline
bsls::Types::Int64 TestAllocatorMonitor::numBlocksTotalChange() const
{
    return d_testAllocator_p->numBlocksTotal() - d_initialTotal;
}

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslma::TestAllocatorMonitor bslma_TestAllocatorMonitor;
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
