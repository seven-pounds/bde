// bsltf_stdstatefulallocator.h                                       -*-C++-*-
#ifndef INCLUDED_BSLTF_STDSTATEFULALLOCATOR
#define INCLUDED_BSLTF_STDSTATEFULALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a minimal standard compliant allocator.
//
//@CLASSES:
//  bsltf::StdStatefulAllocator: standard compliant allocator managing state
//
//@SEE_ALSO: bsltf_stdtestallocator
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an allocator, 'StdStatefulAllocator',
// that defines the minimal interface to comply with section 17.6.3.5
// ([allocator.requirements]) of the C++11 standard, while still providing an
// externally visible and potentially distinct state for each allocator object.
// This type can be used to verify that constructs designed to support a
// standard-compliant allocator access the allocator only through the
// standard-defined interface.
//
// 'StdStatefulAllocator' delegates its operations to 'bslma::TestAllocator'
// (delegate allocator) that is also the sole attribute of this class.  We
// choose to store a test allocator as the attribute constituting state as
// this allows a common test strategy to support testing allocation with BDE
// allocators, and testing the general notion of stateful STL allocators.  The
// key differences between this test allocator and a regular BDE allocator are:
//..
//: o This allocator does not support the 'scoped' allocation model, so that
//:   elements in a container will often have a different allocator to the
//:   container object itself.
//:
//: o This allocator may propagate through copy operations, move operations
//:   and 'swap' operations, depending on how the template is configured as
//:   it is instantiated.
//:
//: o This allocator is not DefaultConstructible, and has no notion of the
//:   default allocator.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing The Support for STL-Compliant Allocator
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will verify that a type supports the use of a
// STL-compliant allocator.
//
// First we define a simple container type intended to be used with a C++03
// standard compliant allocator:
//..
//  template <class TYPE, class ALLOCATOR>
//  class MyContainer {
//      // This container type is parameterized on a standard allocator type
//      // and contains a single object, always initialized, which can be
//      // replaced and accessed.
//
//      // DATA MEMBERS
//      ALLOCATOR  d_allocator;  // allocator used to supply memory (held, not
//                               // owned)
//
//      TYPE      *d_object_p;   // pointer to the contained object
//
//    public:
//      // CONSTRUCTORS
//      MyContainer(const TYPE& object);
//          // Create an container containing the specified 'object', using the
//          // parameterized 'ALLOCATOR' to allocate memory.
//
//      ~MyContainer();
//          // Destroy this container.
//
//      // MANIPULATORS
//      TYPE& object();
//
// // Return a reference providing modifiable access to the object
//          // contained in this container.
//
//      // ACCESSORS
//      const TYPE& object() const;
//          // Return a reference providing non-modifiable access to the object
//          // contained in this container.
//  };
//..
// Then, we define the member functions of 'MyContainer':
//..
//  // CREATORS
//  template <class TYPE, class ALLOCATOR>
//  MyContainer<TYPE, ALLOCATOR>::MyContainer(const TYPE& object)
//  {
//      d_object_p = d_allocator.allocate(1);
//      d_allocator.construct(d_object_p, object);
//  }
//
//  template <class TYPE, class ALLOCATOR>
//  MyContainer<TYPE, ALLOCATOR>::~MyContainer()
//  {
//      d_allocator.destroy(d_object_p);
//      d_allocator.deallocate(d_object_p);
//  }
//
//  // MANIPULATORS
//  template <class TYPE, class ALLOCATOR>
//  TYPE& MyContainer<TYPE, ALLOCATOR>::object()
//  {
//      return *d_object_p;
//  }
//
//  // ACCESSORS
//  template <class TYPE, class ALLOCATOR>
//  const TYPE& MyContainer<TYPE, ALLOCATOR>::object() const
//  {
//      return *d_object_p;
//  }
//..
// Now, we use 'StdStatefulAllocator' to implement a simple test for
// 'MyContainer' to verify it correctly uses a parameterized allocator using
// only the C++03 standard methods:
//..
//  bslma_TestAllocator oa("object", veryVeryVeryVerbose);
//  StdStatefulAllocatorConfigurationGuard stag(&oa);
//  {
//      typedef MyContainer<int, StdStatefulAllocator<int> > Obj;
//
//      Obj mX(2); const Obj& X = mX;
//      assert(sizeof(int) == oa.numBytesInUse());
//
//      assert(X.object() == 2);
//
//      mX.object() = -10;
//      assert(X.object() == -10);
//  }
//
//  assert(0 == oa.numBytesInUse());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace BloombergLP {
namespace bsltf {

                        // ==========================
                        // class StdStatefulAllocator
                        // ==========================

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION = true,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT = true,
          bool  PROPAGATE_ON_CONTAINER_SWAP = true,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT = true>
class StdStatefulAllocator {
    // This allocator implements the minimal interface to comply with section
    // 17.6.3.5 ([allocator.requirements]) of the C++11 standard, while
    // maintaining a distinct object state - in this case a wrapped pointer to
    // a 'bslma::TestAllocator'.  The template is configurable to control its
    // allocator propagation properties, but does not support the BDE "scoped"
    // allocator model, as scoped allocators should never propagate.  Instances
    // of this allocator delegate their operations to the wrapped test
    // allocator that constitutes its state.  Note that meeting only the
    // minimal requirements means that this class is not DefaultConstructible.
    // Note that while we define the various traits used by the C++11 allocator
    // traits facility, they actually mean very little for this component, as
    // it is the consumer of the allocator's responsibility to check and apply
    // the traits correctly, typically by using 'bsl::allocator_traits' to
    // perform all memory allocation tasks rather than using the allocator
    // directly.  The 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' flag is
    // consumed directly though, in the static member function
    // 'select_on_container_copy_construction'.

  private:
    // CLASS DATA
    bslma::TestAllocator *d_allocator_p; // the wrapped test allocator

  public:
    // PUBLIC TYPES
    typedef TYPE value_type;

    // For a minimal allocator, these should all be deducible for a C++11
    // container implementation.  Unfortunately, the C++03 implementation of
    // 'allocator_traits' supported by BDE does not try the leaps of template
    // metaprogramming necessary to deduce these types.  That is left for a
    // future C++11 implementation, where language makes such metaprograms
    // much simpler to write.

    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;

    typedef bsl::integral_constant<bool,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT>
                                        propagate_on_container_copy_assignment;

    typedef bsl::integral_constant<bool, PROPAGATE_ON_CONTAINER_SWAP>
                                                   propagate_on_container_swap;

    typedef bsl::integral_constant<bool,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
                                        propagate_on_container_move_assignment;

    template <class OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some 'OTHER_TYPE',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of 'OTHER_TYPE'.  Note that this allocator type
        // is convertible to and from 'other' for any 'OTHER_TYPE' including
        // 'void'.

        typedef StdStatefulAllocator<
                                 OTHER_TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT> other;
    };

    // CREATORS
    explicit StdStatefulAllocator(bslma::TestAllocator *testAllocator);
        // Create a 'StdStatefulAllocator' object wrapping the specified
        // 'testAllocator'.

    // StdStatefulAllocator(const StdStatefulAllocator& original) = default;
        // Create a 'StdStatefulAllocator' object.  Note that this object will
        // compare equal to the default constructed object, because this type
        // has no state.

    template <class OTHER_TYPE>
    StdStatefulAllocator(const StdStatefulAllocator<
                               OTHER_TYPE,
                               PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                               PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                               PROPAGATE_ON_CONTAINER_SWAP,
                               PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& other);
        // Create a 'StdStatefulAllocator' object wrapping the same test
        // allocator as the specified 'other'.

    // ~StdStatefulAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    // StdStatefulAllocator&
    // operator=(const StdStatefulAllocator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    TYPE *allocate(bslma::Allocator::size_type numElements);
        // Allocate enough (properly aligned) space for the specified
        // 'numElements' of type 'TYPE'.

    void deallocate(TYPE *address, bslma::Allocator::size_type numElements);
        // Return memory previously at the specified 'address' for
        // 'numElements' back to this allocator.  The 'numElements' argument is
        // ignored by this allocator type.  The behavior is undefined unless
        // 'address' was allocated using this allocator object and has not
        // already been deallocated.

    // ACCESSORS
    bslma::TestAllocator *testAllocator() const;
        // Return the address of the test allocator wrapped by this object.

    StdStatefulAllocator select_on_container_copy_construction() const;
        // Return a copy of this object if the 'bool' template parameter
        // 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' is true, and a copy
        // of a 'StdStatefulAllocator' object wrapping the default allocator
        // otherwise.  The behavior is undefined unless the template parameter
        // 'PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION' is true, or unless a
        // 'bslma::TestAllocator' object has been installed as the default
        // allocator.
};

// FREE OPERATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
bool operator==(const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same underlying
    // test allocator, and 'false' otherwise.

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
bool operator!=(const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                const StdStatefulAllocator<
                                 TYPE,
                                 PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                 PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                 PROPAGATE_ON_CONTAINER_SWAP,
                                 PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different underlying
    // test allocators, and 'false' otherwise.


// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // --------------------------
                        // class StdStatefulAllocator
                        // --------------------------

// CREATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
StdStatefulAllocator(bslma::TestAllocator *testAllocator)
: d_allocator_p(testAllocator)
{
    BSLS_ASSERT_SAFE(testAllocator);
}

// CREATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
template <class OTHER_TYPE>
inline
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
StdStatefulAllocator(const StdStatefulAllocator<
                                OTHER_TYPE,
                                PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                PROPAGATE_ON_CONTAINER_SWAP,
                                PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& other)
: d_allocator_p(other.testAllocator())
{
}

// MANIPULATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
TYPE *
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::allocate(
                                       bslma::Allocator::size_type numElements)
{
    return static_cast<TYPE *>(d_allocator_p->allocate(
                     bslma::Allocator::size_type(numElements * sizeof(TYPE))));
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
void StdStatefulAllocator<TYPE,
                          PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                          PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                          PROPAGATE_ON_CONTAINER_SWAP,
                          PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
deallocate(TYPE *address, bslma::Allocator::size_type)
{
    d_allocator_p->deallocate(address);
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
bslma::TestAllocator *
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
testAllocator() const
{
    return d_allocator_p;
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
StdStatefulAllocator<TYPE,
                     PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                     PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                     PROPAGATE_ON_CONTAINER_SWAP,
                     PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>::
select_on_container_copy_construction() const
{
    if (PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION) {
        return *this;                                                 // RETURN
    }

    // else
    return StdStatefulAllocator(dynamic_cast<bslma::TestAllocator *>(
                                          bslma::Default::defaultAllocator()));
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
bool bsltf::operator==(const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                       const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs)
{
    return lhs.testAllocator() == rhs.testAllocator();
}

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
          bool  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
          bool  PROPAGATE_ON_CONTAINER_SWAP,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
inline
bool bsltf::operator!=(const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& lhs,
                       const StdStatefulAllocator<
                                  TYPE,
                                  PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION,
                                  PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT,
                                  PROPAGATE_ON_CONTAINER_SWAP,
                                  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>& rhs)
{
    return lhs.testAllocator() != rhs.testAllocator();
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
