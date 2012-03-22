// bsls_nullptr.t.cpp                                                 -*-C++-*-

#include <bsls_nullptr.h>

#include <bsls_bsltestutil.h>  // for testing only

#include <stdio.h>      // sprintf()
#include <stdlib.h>     // atoi()

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
//-----------------------------------------------------------------------------
// [ 2] class bsls::NullPtr_Impl
// [ 3] typedef bsl::nullptr_t
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [ 4] Usage example
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=========================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

//=============================================================================
//                  GLOBAL HELPER MACROS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                   SUPPORTING FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEF:
        // Concerns:
        //: 1 There exists a type alias 'bls::nullptr_t', aliasing a type that
        //:   represents null pointer literals.
        //: 2 Functions having parameters of type 'bsl::nullptr_t' can be
        //:   invoked only with null pointer literals, '0' and 'NULL'.
        //: 3 Functions having parameters of type 'bsl::nullptr_t' cannot be
        //:   invoked pointers or pointer-to-members, even if they hold the
        //:   null pointer value.
        //
        // Plan:
        //
        // Testing:
        //   bsl::nullptr_t
        // --------------------------------------------------------------------

        struct local {
            // This local utility 'struct' provides a namespace for testing
            // overload resolution for the type under test, 'bsl::nullptr_t'.

            static bool isNullPointer(bsl::nullptr_t) { return true;  }
            static bool isNullPointer(...)            { return false; }
                // Return 'true' is the argument is a null pointer literal, and
                // 'false' otherwise.
        };

        // nulll pointer literals
        static const int s_cZero = 0;
        const int cZero = 0;

        // not null pointer literals
        static void *const Cptr = 0;
        void *ptr = 0;
        int local::*mem = 0;
        static const int& s_zeroRef = 0;
        int zero = 0;

        ASSERT(local::isNullPointer(0));
        ASSERT(local::isNullPointer(NULL));
        ASSERT(local::isNullPointer(false));
        ASSERT(local::isNullPointer(s_cZero));
        ASSERT(local::isNullPointer(cZero));
        ASSERT(local::isNullPointer(1-1));
        ASSERT(local::isNullPointer(0*1));

        ASSERT(!local::isNullPointer(Cptr));
        ASSERT(!local::isNullPointer(ptr));
        ASSERT(!local::isNullPointer(mem));
        ASSERT(!local::isNullPointer((void*)0));
        ASSERT(!local::isNullPointer(zero));
        ASSERT(!local::isNullPointer(s_zeroRef));
        ASSERT(!local::isNullPointer(1));
        ASSERT(!local::isNullPointer(s_zeroRef*1));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING COMPONENT TYPEDEF:
        // Concerns:
        //: 1 The metafunction struct 'bsls::Nullptr_Impl' contains a nested
        //:   alias named 'Type'.
        //: 2 Functions having parameters of type 'bsls::Nullptr_Impl::Type'
        //:   can be invoked only with null pointer literals, '0' and 'NULL'.
        //: 3 Functions having parameters of type 'bsls::Nullptr_Impl::Type'
        //:   cannot be invoked by pointers or pointer-to-members, even if they
        //:   hold the null pointer value.
        //
        // Plan:
        //: 1 Define a local class with two overloads of a static method.
        //:   1 The first overload accepts an argument of type
        //:     'bsls::Nullptr_Impl::Type' and returns 'true' to indicate it
        //:     was selected by overload resolution.
        //:   2 The second overload uses an elipsis paramater list to weakly
        //:     match an argument of fundamental type, and returns 'false'.
        //: 2 Call the static method with null pointer literals, and check that
        //:   the result is 'true' in each case.
        //: 3 Call the static method with various objects, including pointers
        //:   and pointer-to-members, and check that the result is 'false' in
        //:   each case.
        //
        // Testing:
        //   bsls::Nullptr_Impl::Type
        // --------------------------------------------------------------------

#if !defined(BSLS_NULLPTR_USING_NATIVE_NULLPTR_T)
        struct local {
            // This local utility 'struct' provides a namespace for testing
            // overload resolution for the type under test,
            // 'bsls::Nullptr_Impl::Type'.

            static bool isNullPointer(bsls::Nullptr_Impl::Type) { return true; }
            static bool isNullPointer(...)                    { return false; }
                // Return 'true' is the argument is a null pointer literal, and
                // 'false' otherwise.
        };

        // nulll pointer literals
        static const int s_cZero = 0;
        const int cZero = 0;

        // not null pointer literals
        static void *const Cptr = 0;
        void *ptr = 0;
        int local::*mem = 0;
        static const int& s_zeroRef = 0;
        int zero = 0;

        ASSERT(local::isNullPointer(0));
        ASSERT(local::isNullPointer(NULL));
        ASSERT(local::isNullPointer(false));
        ASSERT(local::isNullPointer(s_cZero));
        ASSERT(local::isNullPointer(cZero));
        ASSERT(local::isNullPointer(1-1));
        ASSERT(local::isNullPointer(0*1));

        ASSERT(!local::isNullPointer(Cptr));
        ASSERT(!local::isNullPointer(ptr));
        ASSERT(!local::isNullPointer(mem));
        ASSERT(!local::isNullPointer((void*)0));
        ASSERT(!local::isNullPointer(zero));
        ASSERT(!local::isNullPointer(s_zeroRef));
        ASSERT(!local::isNullPointer(1));
        ASSERT(!local::isNullPointer(s_zeroRef*1));

#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The component is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Provide a function taking an argument of type 'bsl::nullptr_t',
        //:   and then call this function with the literal value '0'.  Confirm
        //:   that the code compiles, and returns the expected value.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        struct local {
            static void test(bsl::nullptr_t) { }
        };

        local::test(0);

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
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
