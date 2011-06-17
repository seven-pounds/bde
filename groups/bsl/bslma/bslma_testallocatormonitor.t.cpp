// bslma_testallocatormonitor.t.cpp
#include <bslma_testallocatormonitor.h>

#include <bsl_string.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslmf_assert.h>

#include <cstring>     // memset()
#include <cstdlib>     // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a ...
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [  ] bslma_TestAllocatorMonitor(const bslma_TestAllocator& tA);
//
// ACCESSORS
// [  ] bool isInUseDown() const;
// [  ] bool isInUseSame() const;
// [  ] bool isInUseUp() const;
// [  ] bool isMaxSame() const;
// [  ] bool isMaxUp() const;
// [  ] bool isTotalSame() const;
// [  ] bool isTotalUp() const;
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [  ] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [  ] CONCERN: All accessor methods are declared 'const'.
// [  ] CONCERN: There is no temporary allocation from any allocator.
// [  ] CONCERN: Precondition violations are detected when enabled.

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

class MyClass
{
    // This class takes an allocator and uses it minimally (one byte is
    // (allocated on construction and deallocated on destruction) in support
    // of the "Usage" example.

    // DATA
    void            *d_data_p;
    bslma_Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit MyClass(bslma_Allocator *basicAllocator = 0);
    ~MyClass();
};

// CREATORS
MyClass::MyClass(bslma_Allocator *basicAllocator)
: d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_data_p = d_allocator_p->allocate(1);
}

MyClass::~MyClass()
{
    d_allocator_p->deallocate(d_data_p);
}

typedef MyClass Obj;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

// JSL: Do we want to move this string to the component of bsl::string itself?
// JSL: e.g.,  #define BSLSTL_LONG_STRING ...   TBD!

#ifdef BSLS_PLATFORM__CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// When testing a class, two typical concerns are:
//: o Is all memory allocated from the supplied allocator?
//: o Is memory leaked?
//
// To address these concerns, the typical testing pattern is to first creates a
// trio of 'bslma_TestAllocator' objects, and install two as the global and
// default allocators.  The remaining allocator will be supplied to the object.
//..
    {
        bslma_TestAllocator ga("global",  veryVeryVeryVerbose);
        bslma_TestAllocator da("default", veryVeryVeryVerbose);
        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma_Default::setGlobalAllocator(&ga);
        bslma_Default::setDefaultAllocatorRaw(&da);
//..
// Then, one creates a trio of 'bslma_TestAllocatorMonitor' objects to capture
// the current state (unused) of each of the 'bslma_TestAllocator' objects.
//..
        bslma_TestAllocatorMonitor gam(ga), dam(da), oam(oa);
//..
// Next, one creates an object of the class under test, supplying the remaining
// allocator, and subjects the object the relevant operations.
//..
        {
            Obj obj(&oa);

            // Use 'obj'.
            // . . .
            // . . .

            // 'obj' destroyed at end of scope
        }
//..
// Finally, after the object is destroyed, one accesses the montior objects to
// obtain the state of the underlying allocators for comparison to the
// expected state.
//..
        ASSERT(true == gam.isTotalSame()); // global  allocator was not used
        ASSERT(true == dam.isTotalSame()); // default allocator was not used

        ASSERT(true == oam.isTotalUp());   // object  allocator was used
        ASSERT(true == oam.isMaxUp());     // object  allocator was used
        ASSERT(true == oam.isInUseSame()); // all memory was returned
    }
//..
//
///Example 2: Strings
//..
    {
        bslma_TestAllocator        oa("object",  veryVeryVeryVerbose);
        bslma_TestAllocatorMonitor oam1(oa);

        bsl::string s;
        ASSERT(true  == oam1.isTotalSame()); // no allocation by default CTOR

        s = "a";
        ASSERT(true  == oam1.isTotalSame()); // no allocation (thanks to
                                             // short string optimization)

        s = SUFFICIENTLY_LONG_STRING;
        ASSERT(true == oam1.isTotalUp()); // allocator was used
        ASSERT(true == oam1.isMaxUp());   // allocator was used
        ASSERT(true == oam1.isInUseUp()); // allocator was used

        bslma_TestAllocatorMonitor oam2(oa);  // capture current state of
                                              // object allocator

        s = "";
        ASSERT(true == oam2.isTotalSame()); // no memory was released
        ASSERT(true == oam2.isMaxSame());   // no memory was released
        ASSERT(true == oam2.isInUseSame()); // no memory was released
    }
//..

      } break;
      case 1: {

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}
