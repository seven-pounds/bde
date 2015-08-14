// bdlat_nullablevaluefunctions.t.cpp                                 -*-C++-*-

#include <bdlat_nullablevaluefunctions.h>

#include <bdlb_nullablevalue.h>
#include <bslmf_if.h>                 // for testing only
#include <bslmf_issame.h>             // for testing only

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::atoi;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                        // ===========================
                        // class GetValue<LVALUE_TYPE>
                        // ===========================

template <class LVALUE_TYPE>
class GetValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE *d_lValue_p;  // held, not owned

  public:
    // CREATORS
    explicit GetValue(LVALUE_TYPE *lValue);

    // ACCESSORS
    int operator()(const LVALUE_TYPE& object) const;
        // Assign 'object' to '*d_destination_p'.

    template <class RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Do nothing.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <class RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    const RVALUE_TYPE& d_value;  // held, not owned

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    int operator()(RVALUE_TYPE *object) const;
        // Assign 'd_value' to '*object'.

    template <class LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Do nothing.
};

                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <class LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <class LVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object) const
{
    *d_lValue_p = object;
    return 0;
}

template <class LVALUE_TYPE>
template <class RVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <class RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <class RVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object) const
{
    *object = d_value;
    return 0;
}

template <class RVALUE_TYPE>
template <class LVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *object) const
{
    return -1;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace Obj = bdeat_NullableValueFunctions;

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

class PrintValue {
    // This function will print values to the specified output stream.

    // PRIVATE DATA MEMBERS
    bsl::ostream *d_stream_p;

    // PRIVATE TYPES
    struct IsNotNullableValueType { };
    struct IsNullableValueType    { };

    // PRIVATE OPERATIONS
    template <class TYPE>
    int execute(const TYPE& value, IsNotNullableValueType)
    {
        enum { SUCCESS = 0 };

        (*d_stream_p) << value;

        return SUCCESS;
    }

    template <class TYPE>
    int execute(const TYPE& value, IsNullableValueType)
    {
        enum { SUCCESS = 0 };

        if (bdeat_NullableValueFunctions::isNull(value)) {
            (*d_stream_p) << "NULL";

            return SUCCESS;                                           // RETURN
        }

        return bdeat_NullableValueFunctions::accessValue(value, *this);
    }

  public:
    // CREATORS
    PrintValue(bsl::ostream *stream)
    : d_stream_p(stream)
    {
    }

    // OPERATIONS
    template <class TYPE>
    int operator()(const TYPE& value)
    {
        typedef typename
        bslmf::If<
            bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
            IsNullableValueType,
            IsNotNullableValueType>::Type Toggle;

        return execute(value, Toggle());
    }
};  // end 'class PrintValue'
//..
// The 'PrintValue' function class can be used for types that expose "nullable"
// behavior through the 'bdeat_NullableValueFunctions' 'namespace' (e.g.,
// 'bdlb::NullableValue') and any other type that has 'operator<<' defined for
// it.  For example:
//..
void usageExample(bsl::ostream& os)
{
    PrintValue printValue(&os);

    int intScalar = 123;

    printValue(intScalar);  // expected output: '123'

    bdlb::NullableValue<int> intNullable;

    printValue(intNullable);  // expected output: 'NULL'

    intNullable.makeValue(321);

    printValue(intNullable);  // expected output: '321'
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
        case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   That the usage example compiles and runs as expected.
        //
        // Plan:
        //   Copy-paste the example, change the assert into ASSERT,
        //   and pass a string stream instead of 'bsl::cout' in order to check
        //   the print format.
        //
        // Testing:
        //   Usage Example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        bsl::stringstream ss;
        usageExample(ss);
        bsl::string s; ss >> s;

        LOOP_ASSERT(s, "123NULL321" == s);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsNullableValue
        //   struct ValueType
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == Obj::IsNullableValue<int>::VALUE);
        ASSERT(1 == Obj::IsNullableValue<bdlb::NullableValue<int> >::VALUE);

        typedef Obj::ValueType<bdlb::NullableValue<int> >::Type ValueType;
        ASSERT(1 == (bslmf::IsSame<ValueType, int>::VALUE));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHOD FORWARDING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD FORWARDING TEST" << endl
                          << "======================" << endl;

        bdlb::NullableValue<int> mV;  const bdlb::NullableValue<int>& V = mV;

        Obj::makeValue(&mV); ASSERT(!Obj::isNull(V));

        int              value;
        GetValue<int>    getter(&value);
        AssignValue<int> setter1(33);
        AssignValue<int> setter2(44);

        Obj::manipulateValue(&mV, setter1);
        Obj::accessValue(V, getter); ASSERT(33 == value);

        Obj::manipulateValue(&mV, setter2);
        Obj::accessValue(V, getter); ASSERT(44 == value);

        mV.reset();
        ASSERT(Obj::isNull(V));
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
