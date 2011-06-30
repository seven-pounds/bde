// bslalg_swaputil.t.cpp                                              -*-C++-*-

#include <bslalg_swaputil.h>

#include <stdio.h>      // 'printf'

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// bslalg_SwapUtil public interface:
// [ 2] void swap(T *a, T *b);
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
int testStatus = 0;

namespace {
    void aSsErT(int c, const char *s, int i) {
        if (c) {
            printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
            if (testStatus >= 0 && testStatus <= 100) ++testStatus;
        }
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { printf("%s: %d\t%s: %c\t%s: %c\n", #I, I, #J, J, #K, K); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
//-----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

struct SwapTester {
    bool swapped;

    SwapTester()
    : swapped(false)
    {}
};

inline
void swap(SwapTester& a, SwapTester& b)
{
    a.swapped = b.swapped = true;
}

struct SwapContainer
{
    SwapTester m_t;

    void swap(SwapContainer & other)
    {
        bslalg_SwapUtil::swap(&m_t, &other.m_t);
    }
};

inline
void swap(SwapContainer & a, SwapContainer & b)
{
    a.swap(b);
}

}

using namespace BloombergLP;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
//
///Usage
///-----
///Example 1: using 'bslalg_SwapUtil::swap'
/// - - - - - - - - - - - - - - - - - - - -
// In this section we show the intended usage of this component.  We start by
// defining a class 'Container' in the 'xyz' namespace.  Further we assume that 
// 'Container' has some expensive-to-copy data, so we provide a custom 'swap'
// algorithm to efficiently swap the data between a two objects this class by
// defining a 'swap' method and a 'swap' free function.
//..
namespace xyz {

class Container {
  private:
    int d_expensiveData;

  public:
    void swap(Container& other);
};

void swap(Container& a, Container& b);

}
//..
// Note that the free function 'swap' is overloaded in the namespace of the
// class 'Container', which is 'xyz'.
//
// Next, we implemente the 'swap' method using the 'bslalg_SwapUtil::swap' to
// swap the individual data elements:
//..
inline
void xyz::Container::swap(Container& other)
{
    bslalg_SwapUtil::swap(&d_expensiveData, &other.d_expensiveData);
}
//..
// Note that calling 'bslalg_SwapUtil::swap' is equivalent to making the
// 'bsl::swap' available in the current scope by doing 'using bsl::swap' and
// making a subsequent call to an unqualified 'swap' function.
//
// Then, we implement the 'swap' free function:
//..
inline
void xyz::swap(Container& a, Container& b)
{
    a.swap(b);
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally we can use 'bslalg_SwapUtil::swap' to swap two objects of class
// 'xyz::Container':
//..
xyz::Container c1, c2;

bslalg_SwapUtil::swap(&c1, &c2);
//..
// The above code correctly calls the 'xyz::swap' overload for the 'Container'
// class.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING swap
        //
        // Concerns:
        //: 1 'bslalg_SwapUtil::swap' calls the 'swap' found by ADL in the
        //:   namespace of a class for the class that provides its own 'swap'
        //:   overload.
        //: 2 'bslald_SwapUtil::swap' calls the generic 'bsl::swap' for a class
        //:   that doesn't provide its own 'swap' overload.
        //
        // Plan:
        //: 1 Create two objects of type 'SwapTester' which provides its own
        //:   overload of 'swap'.  Call the 'bslalg_SwapUtil::swap' on them and
        //:   verify that the overloaded 'swap' has been called.
        //: 2 Call 'bslalg_SwapUtil::swap' on two ints.  Since 'int' doesn't
        //:   have its own 'swap' overload, the generic 'swap' has to be found.
        //
        // Testing:
        //   void swap(T *a, T *b);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'swap'\n");

        SwapTester a;
        SwapTester b;

        ASSERT(!a.swapped);
        ASSERT(!b.swapped);

        bslalg_SwapUtil::swap(&a, &b);

        ASSERT(a.swapped);
        ASSERT(b.swapped);

        int c = 10;
        int d = 20;

        bslalg_SwapUtil::swap(&c, &d);

        ASSERT(c == 20);
        ASSERT(d == 10);

        if (verbose) printf("\nTesting 'swap' for a 'container' class\n");

        SwapContainer ca;
        SwapContainer cb;
        ASSERT(!ca.m_t.swapped);
        ASSERT(!cb.m_t.swapped);

        bslalg_SwapUtil::swap(&ca, &cb);

        ASSERT(ca.m_t.swapped);
        ASSERT(cb.m_t.swapped);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   The 'swap' utility function works as intended.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int c = 10;
        int d = 20;

        bslalg_SwapUtil::swap(&c, &d);

        ASSERT(c == 20);
        ASSERT(d == 10);

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
