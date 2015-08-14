// bdlmxxx_convert.t.cpp                                              -*-C++-*-

#include <bdlmxxx_convert.h>

#include <bdlmxxx_choice.h>
#include <bdlmxxx_choicearray.h>
#include <bdlmxxx_choicearrayitem.h>
#include <bdlmxxx_elemattrlookup.h>
#include <bdlmxxx_elemref.h>
#include <bdlmxxx_list.h>
#include <bdlmxxx_row.h>
#include <bdlmxxx_table.h>

#include <bslmf_removecvq.h>

#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Ideally, we would like to test every method in 'bdlmxxx::Convert' with every
// combination of bdem types.  However, there are (at last count) 32
// bdem types, which would necessitate 1024 test vectors in order to test each
// combination only once.  To simplify the problem, we partition the 32 types
// into the following categories:
//
// 1. Numeric (integral and floating-point) types, including bool and char
// 2. date and time types
// 3. string
// 4. array types
// 5. aggregate types
//
// The conversion matrix among the categories has only 25 entries, as follows:
//
//           to category
//            1 2 3 4 5
//          +----------
//        1 | A - A - -     KEY:
//  from  2 | - / A - -     - = All conversion will fail
//  cate- 3 | / / A - -     / = Some conversions will succeed
//  gory  4 | - - A / -     A = All conversions will succeed
//        5 | - - A - /
//
// Note that the majority of conversions will fail.  Failed conversions can
// be tested without creating a test vector for each pair: One function
// supplies values belonging to the "from" category and calls a templated
// function that attempts conversion to each type in the "to" category, thus
// achieving the full cross-product with minimal input.  This leaves a
// manageable number of successful conversions to test using test vectors
// that supply both input and expected output values.
//
// For each conversion test (whether the expected result is successful or
// unsuccessful), we want to test at least two values plus the null value (if
// any).  All of the 'bdlmxxx::Convert' methods are tested in each test case,
// using the same input and expected-output data.  Test vectors are expressed
// as calls to template functions, where the input type and output type are
// supplied as template parameters.  A set of test vectors is thus a series of
// function calls, where each "line" in the set may involve different types
// than the previous line.
//
// NOTE: because this test makes very heavy use of template instantiation, it
// can take a very long time to compile (over 20 minutes with gcc).  For this
// reason, some mostly-redundant tests are #ifdef'd out.  These tests can be
// turned back on by defining the 'LONG_TEST' preprocessor macro (i.e. by
// compiling with '-DLONG_TEST'.
//
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

void aSsErTq(int c, const char *s, int i) {
    // Special assert that does not print unless it's on 'veryVeryVerbose'
    // mode.
    if (c) {
        if (veryVeryVerbose) {
            bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                      << "    (failed)" << bsl::endl;
        }
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X)       { aSsErT(!(X), #X, __LINE__); }
# define ASSERT_QUIET(X) { aSsErTq(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

// Sun compilers have issues with this test driver, so we disable some test
// cases in some situations.
#if defined(BSLS_PLATFORM_CMP_SUN)
#if defined(BDE_BUILD_TARGET_OPT)
// We have to skip all cases when building optimized on Sun, or the compiler
// runs out of memory.
#undef TEST_IS_ENABLED
#define TEST_IS_ENABLED(num) 0
#elif BSLS_PLATFORM_CMP_VER_MAJOR>=0x5100
// We have to skip case 5 for cc-5.10, or the compiler runs out of memory.
#undef TEST_IS_ENABLED
#define TEST_IS_ENABLED(num)                                                  \
        ((defined(SINGLE_TEST) && SINGLE_TEST == (num)) || (5 != (num)))
#endif
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

typedef bdlmxxx::ElemType ET;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

static const bool  BOOL_ZERO = false;
static const bool  BOOL_V1   = true;

static const char  CHAR_ZERO = 0;
static const char  CHAR_V1   = 45;
static const char  CHAR_V2   = (char) -5;

static const short SHORT_NULL = bdltuxxx::Unset<short>::unsetValue();
static const short SHORT_ZERO = 0;
static const short SHORT_V1   = 46;
static const short SHORT_V2   = -300;

static const int   INT_NULL = bdltuxxx::Unset<int>::unsetValue();
static const int   INT_ZERO = 0;
static const int   INT_V1   = 1234567;
static const int   INT_V2   = -7654321;

static const Int64 INT64_NULL = bdltuxxx::Unset<Int64>::unsetValue();
static const Int64 INT64_ZERO = 0;
static const Int64 INT64_V1   = 77;
static const Int64 INT64_V2   = -0x123456789abcdef0LL;

static const float FLOAT_NULL = bdltuxxx::Unset<float>::unsetValue();
static const float FLOAT_ZERO = 0.0;
static const float FLOAT_V1   = 125.125;
static const float FLOAT_V2   = 6.5e-43;

static const double DOUBLE_NULL = bdltuxxx::Unset<double>::unsetValue();
static const double DOUBLE_ZERO = 0.0;
static const double DOUBLE_V1   = -9.75;
static const double DOUBLE_V2   = 1.2345e9;

static const unsigned short USHORT_ZERO = 0;
static const unsigned short USHORT_V1   = 124;
static const unsigned short USHORT_V2   = 65000;

static const unsigned int   UINT_ZERO = 0;
static const unsigned int   UINT_V1   = 1357911;
static const unsigned int   UINT_V2   = 3;

static const Uint64 UINT64_ZERO = 0;
static const Uint64 UINT64_V1   = 2006;
static const Uint64 UINT64_V2   = 0xfedcba9876543210ULL;

static const bdlt::Date DATE_NULL(bdltuxxx::Unset<bdlt::Date>::unsetValue());
static const bdlt::Date DATE_V1(1996,7,8);
static const bdlt::Date DATE_V2(1997,8,9);

static const bdlt::Time TIME_NULL(bdltuxxx::Unset<bdlt::Time>::unsetValue());
static const bdlt::Time TIME_V1(11,12,13,14);
static const bdlt::Time TIME_V2(12,13,14,15);

static const bdlt::Datetime
                       DATETIME_NULL(bdltuxxx::Unset<bdlt::Datetime>::unsetValue());
static const bdlt::Datetime DATETIME_V1(bdlt::Date(1981,2,3),
                                       bdlt::Time(1,2,3,4));
static const bdlt::Datetime DATETIME_V2(bdlt::Date(1982,3,4),
                                       bdlt::Time(2,3,4,5));
static const bdlt::Datetime DATETIME_V3(DATE_V1);
static const bdlt::Datetime DATETIME_V4(DATE_V2);

static const bdlt::DateTz DATETZ_NULL(bdltuxxx::Unset<bdlt::DateTz>::unsetValue());
static const bdlt::DateTz DATETZ_V1(DATE_V1,-1);
static const bdlt::DateTz DATETZ_V2(DATE_V2, 2);
static const bdlt::DateTz DATETZ_V3(DATE_V1, 0);
static const bdlt::DateTz DATETZ_V4(DATE_V2, 0);

static const bdlt::TimeTz TIMETZ_NULL(bdltuxxx::Unset<bdlt::TimeTz>::unsetValue());
static const bdlt::TimeTz TIMETZ_V1(TIME_V1,-1);
static const bdlt::TimeTz TIMETZ_V2(TIME_V2, 2);
static const bdlt::TimeTz TIMETZ_V3(TIME_V1, 0);
static const bdlt::TimeTz TIMETZ_V4(TIME_V2, 0);

static const bdlt::DatetimeTz
                   DATETIMETZ_NULL(bdltuxxx::Unset<bdlt::DatetimeTz>::unsetValue());
static const bdlt::DatetimeTz DATETIMETZ_V1(DATETIME_V1,-1);
static const bdlt::DatetimeTz DATETIMETZ_V2(DATETIME_V2, 2);
static const bdlt::DatetimeTz DATETIMETZ_V3(DATETIME_V1, 0);
static const bdlt::DatetimeTz DATETIMETZ_V4(DATETIME_V2, 0);

//=============================================================================
//                  GLOBAL HELPER ENTITIES FOR TESTING
//-----------------------------------------------------------------------------

template <class CPPTYPE>
struct BdemEnum {
    // Return the 'bdlmxxx::ElemType' enumeration for a C++ type.
    enum { VALUE = ET::BDEM_VOID };
};

template <> struct BdemEnum<char>
{ enum { VALUE = ET::BDEM_CHAR }; };
template <> struct BdemEnum<short>
{ enum { VALUE = ET::BDEM_SHORT }; };
template <> struct BdemEnum<int>
{ enum { VALUE = ET::BDEM_INT }; };
template <> struct BdemEnum<Int64>
{ enum { VALUE = ET::BDEM_INT64 }; };
template <> struct BdemEnum<float>
{ enum { VALUE = ET::BDEM_FLOAT }; };
template <> struct BdemEnum<double>
{ enum { VALUE = ET::BDEM_DOUBLE }; };
template <> struct BdemEnum<bsl::string>
{ enum { VALUE = ET::BDEM_STRING }; };
template <> struct BdemEnum<bdlt::Datetime>
{ enum { VALUE = ET::BDEM_DATETIME }; };
template <> struct BdemEnum<bdlt::Date>
{ enum { VALUE = ET::BDEM_DATE }; };
template <> struct BdemEnum<bdlt::Time>
{ enum { VALUE = ET::BDEM_TIME }; };
template <> struct BdemEnum<bsl::vector<char> >
{ enum { VALUE = ET::BDEM_CHAR_ARRAY }; };
template <> struct BdemEnum<bsl::vector<short> >
{ enum { VALUE = ET::BDEM_SHORT_ARRAY }; };
template <> struct BdemEnum<bsl::vector<int> >
{ enum { VALUE = ET::BDEM_INT_ARRAY }; };
template <> struct BdemEnum<bsl::vector<Int64> >
{ enum { VALUE = ET::BDEM_INT64_ARRAY }; };
template <> struct BdemEnum<bsl::vector<float> >
{ enum { VALUE = ET::BDEM_FLOAT_ARRAY }; };
template <> struct BdemEnum<bsl::vector<double> >
{ enum { VALUE = ET::BDEM_DOUBLE_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bsl::string> >
{ enum { VALUE = ET::BDEM_STRING_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bdlt::Datetime> >
{ enum { VALUE = ET::BDEM_DATETIME_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bdlt::Date> >
{ enum { VALUE = ET::BDEM_DATE_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bdlt::Time> >
{ enum { VALUE = ET::BDEM_TIME_ARRAY }; };
template <> struct BdemEnum<bdlmxxx::List>
{ enum { VALUE = ET::BDEM_LIST }; };
template <> struct BdemEnum<bdlmxxx::Table>
{ enum { VALUE = ET::BDEM_TABLE }; };

template <> struct BdemEnum<bool>
{ enum { VALUE = ET::BDEM_BOOL }; };
template <> struct BdemEnum<bdlt::DatetimeTz>
{ enum { VALUE = ET::BDEM_DATETIMETZ }; };
template <> struct BdemEnum<bdlt::DateTz>
{ enum { VALUE = ET::BDEM_DATETZ }; };
template <> struct BdemEnum<bdlt::TimeTz>
{ enum { VALUE = ET::BDEM_TIMETZ }; };
template <> struct BdemEnum<bsl::vector<bool> >
{ enum { VALUE = ET::BDEM_BOOL_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bdlt::DatetimeTz> >
{ enum { VALUE = ET::BDEM_DATETIMETZ_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bdlt::DateTz> >
{ enum { VALUE = ET::BDEM_DATETZ_ARRAY }; };
template <> struct BdemEnum<bsl::vector<bdlt::TimeTz> >
{ enum { VALUE = ET::BDEM_TIMETZ_ARRAY }; };
template <> struct BdemEnum<bdlmxxx::Choice>
{ enum { VALUE = ET::BDEM_CHOICE }; };
template <> struct BdemEnum<bdlmxxx::ChoiceArray>
{ enum { VALUE = ET::BDEM_CHOICE_ARRAY }; };

template <> struct BdemEnum<bdlmxxx::Row>
{ enum { VALUE = ET::BDEM_ROW }; };
template <> struct BdemEnum<bdlmxxx::ChoiceArrayItem>
{ enum { VALUE = ET::BDEM_CHOICE_ARRAY_ITEM }; };

template <class T>
class ConvertibleTo {
    // Object that's convertible to 'T'.
    T d_value;
  public:
    ConvertibleTo() : d_value() { }
    explicit ConvertibleTo(const T& value) : d_value(value) { }
    operator T() const { return d_value; }

    bool operator==(const ConvertibleTo& rhs) const {
        return d_value == rhs.d_value;
    }
};

template <class T>
class ConvertibleFrom {
    // Object that's convertible From 'T'.
    T d_value;
  public:
    ConvertibleFrom() : d_value() { }
    ConvertibleFrom(const T& value) : d_value(value) { }

    const T& value() const { return d_value; }

    bool operator==(const ConvertibleFrom& rhs) const {
        return d_value == rhs.d_value;
    }

    bool operator!=(const ConvertibleFrom& rhs) const {
        return d_value != rhs.d_value;
    }
};

template <class T>
bsl::ostream& operator<<(bsl::ostream& os, const ConvertibleFrom<T>& v)
{
    return os << v.value();
}

template <class T>
class TestValue {
    // This class hold an object of type 'T' which can be modified then
    // "reloaded" with an initial value.  The test driver obtains a modifiable
    // reference to the held object, modifies it, then uses the 'reset' method
    // to set it back to its initial value before performing another test.
    // The 'isChanged' method conveniently reports if the current value is
    // different from the initial value.

    T d_initial;
    T d_current;

  public:
    TestValue(const T& v) : d_initial(v), d_current(v) { }
    void reset() { d_current = d_initial; }
    T& value() { return d_current; }
    const T& value() const { return d_current; }
    const T& initialValue() const { return d_initial; }
    bool isChanged() const { return d_current != d_initial; }
};

template <>
class TestValue<bdlmxxx::Row> {
    // Because 'bdlmxxx::Row' is not a value-semantic type, we need a specialized
    // implementation of 'TestValue'.

    bdlmxxx::List d_initial;
    bdlmxxx::List d_current;

  public:
    TestValue(const bdlmxxx::Row& v) : d_initial(v), d_current(v) { }
    void reset() { d_current = d_initial; }
    bdlmxxx::Row& value() { return d_current.row(); }
    const bdlmxxx::Row& value() const { return d_current.row(); }
    const bdlmxxx::Row& initialValue() const { return d_initial.row(); }
    bool isChanged() const { return d_current != d_initial; }
};

template <>
class TestValue<bdlmxxx::ChoiceArrayItem> {
    // Because 'bdlmxxx::ChoiceArrayItem' is not a value-semantic type, we need a
    // specialized implementation of 'TestValue'.

    bdlmxxx::Choice d_initial;
    bdlmxxx::Choice d_current;

  public:
    TestValue(const bdlmxxx::ChoiceArrayItem& v) : d_initial(v), d_current(v) { }
    void reset() { d_current = d_initial; }
    bdlmxxx::ChoiceArrayItem& value() { return d_current.item(); }
    const bdlmxxx::ChoiceArrayItem& value() const { return d_current.item(); }
    const bdlmxxx::ChoiceArrayItem& initialValue() const
        { return d_initial.item(); }
    bool isChanged() const { return d_current != d_initial; }
};

template <class T, bsl::size_t NUM_ELEMS>
class TestValue<T[NUM_ELEMS]> {
    // This class hold an object of type 'T' which can be modified then
    // "reloaded" with an initial value.  The test driver obtains a modifiable
    // reference to the held object, modifies it, then uses the 'reset' method
    // to set it back to its initial value before performing another test.
    // The 'isChanged' method conveniently reports if the current value is
    // different from the initial value.

    typedef typename bslmf::RemoveCvq<T>::Type ItemType;
    typedef ItemType ArrayType[NUM_ELEMS];
    ArrayType d_initial;
    ArrayType d_current;

  public:
    TestValue(const ArrayType& v) { }
    void reset() { }
    ArrayType& value() { return d_current; }
    const ArrayType& value() const { return d_current; }
    const ArrayType& initialValue() const { return d_initial; }
    bool isChanged() const { return false; }
};

template <class FROMTYPE, class TOTYPE>
void testConversions(int             line,
                     bool            expectedSuccess,
                     const TOTYPE&   initial,
                     const FROMTYPE& from,
                     const TOTYPE&   expectedResult)
    // Assert that conversion (using each of the 'bdlmxxx::Convert' methods) from
    // the specified 'from' value to the specified 'TOTYPE' will succeed and
    // produce the specified 'expectedResult'.  If an assertion fails, 'line'
    // will be printed before the error message.
{
#define ASSERT_EXPECTED_RESULT                                           \
    if ((0 == conversionStatus) != expectedSuccess) {                    \
        if (veryVeryVerbose) {                                           \
            bsl::cout << "line: " << line << "\n";                       \
            aSsErT(1, (expectedSuccess ?                                 \
                   "0 == conversionStatus" : "0 != conversionStatus"),   \
                   __LINE__);                                            \
        }                                                                \
        else {                                                           \
            aSsErTq(1, (expectedSuccess ?                                \
                   "0 == conversionStatus" : "0 != conversionStatus"),   \
                   __LINE__);                                            \
        }                                                                \
    } else if (expectedSuccess) {                                        \
        LOOP3_ASSERT(line,                                               \
                     toTestValue.value(),                                \
                     expectedResult,                                     \
                     toTestValue.value() == expectedResult);             \
    } else {                                                             \
        LOOP_ASSERT(line, ! toTestValue.isChanged());                    \
    }

#define ASSERT_NULL_EXPECTED_RESULT                                      \
    if ((0 == conversionStatus) != expectedSuccess) {                    \
        if (veryVeryVerbose) {                                           \
            bsl::cout << "line: " << line << "\n";                       \
            aSsErT(1, (expectedSuccess ?                                 \
                   "0 == conversionStatus" : "0 != conversionStatus"),   \
                   __LINE__);                                            \
        }                                                                \
        else {                                                           \
            aSsErTq(1, (expectedSuccess ?                                \
                   "0 == conversionStatus" : "0 != conversionStatus"),   \
                   __LINE__);                                            \
        }                                                                \
    } else if (expectedSuccess) {                                        \
        LOOP3_ASSERT(line,                                               \
                     toNullTestValue.value(),                            \
                     expectedResult,                                     \
                     toNullTestValue.value() == expectedResult);         \
    } else {                                                             \
        LOOP_ASSERT(line, ! toNullTestValue.isChanged());                \
    }

#define ASSERT_FAILED_CONVERSION \
    if (0 == conversionStatus) { LOOP_ASSERT(line, 0 != conversionStatus) }  \
    else { LOOP_ASSERT(line, ! toTestValue.isChanged()) }

    TestValue<TOTYPE> toTestValue(initial);

    TestValue<FROMTYPE> fromNullTestValue(from);
    FROMTYPE& fromUnset = fromNullTestValue.value();
    const FROMTYPE& FROM_UNSET = fromUnset;

    const void *const fromVoidPtr = &from;
    const ET::Type fromTypeEnum = (ET::Type) BdemEnum<FROMTYPE>::VALUE;
    const bdlmxxx::Descriptor *const fromTypeDesc =
        bdlmxxx::ElemAttrLookup::lookupTable()[fromTypeEnum];

    const int fromWord1 = 0, fromNullWord1 = 1;
    int fromWord2 = 0, fromNullWord2 = 1;
    int toWord1 = 0, toNullWord1 = 1;
    const int NULLNESS_OFFSET = 0;

    const bdlmxxx::ConstElemRef fromConstElemRef(fromVoidPtr,
                                             fromTypeDesc,
                                             &fromWord1,
                                             NULLNESS_OFFSET);
    const bdlmxxx::ConstElemRef fromNullConstElemRef(&FROM_UNSET,
                                                 fromTypeDesc,
                                                 &fromNullWord1,
                                                 NULLNESS_OFFSET);

    const bdlmxxx::ElemRef fromElemRef(const_cast<void*>(fromVoidPtr),
                                   fromTypeDesc,
                                   &fromWord2,
                                   NULLNESS_OFFSET);

    const bdlmxxx::ElemRef fromNullElemRef((void *) &fromUnset,
                                       fromTypeDesc,
                                       &fromNullWord2,
                                       NULLNESS_OFFSET);

    if (fromNullElemRef.isBound()) {
        fromNullElemRef.makeNull();
    }

    TOTYPE toUnset; // const TOTYPE& TO_UNSET = toUnset; // TO_UNSET never used
    TestValue<TOTYPE> toNullTestValue(toUnset);

    TOTYPE& toObj     = toTestValue.value();
    TOTYPE& toNullObj = toNullTestValue.value();
    void *const toVoidPtr     = (void*) &toObj;
    void *const toNullVoidPtr = (void*) &toNullObj;
    const ET::Type toTypeEnum = (ET::Type) BdemEnum<TOTYPE>::VALUE;
    const bdlmxxx::Descriptor *const toTypeDesc =
        bdlmxxx::ElemAttrLookup::lookupTable()[toTypeEnum];
    bdlmxxx::ElemRef toElemRef(toVoidPtr,
                           toTypeDesc,
                           &toWord1,
                           NULLNESS_OFFSET);
    bdlmxxx::ElemRef toNullElemRef(toNullVoidPtr,
                               toTypeDesc,
                               &toNullWord1,
                               NULLNESS_OFFSET);
#ifdef LONG_TEST
    const TOTYPE& toConstObj = toTestValue.value();
    const void *const toConstVoidPtr = &toConstObj;
    bdlmxxx::ConstElemRef toConstElemRef(toElemRef);
    bdlmxxx::ConstElemRef toNullConstElemRef(toElemRef);
#endif

    int conversionStatus;

    conversionStatus = bdlmxxx::Convert::convert(&toObj, from);
    ASSERT_EXPECTED_RESULT;
    if (veryVeryVerbose) {
        bsl::cout << "AFTER:  ";
        P_(toObj)
        if (expectedSuccess) { P_(expectedResult) }
        P(from)
    }

#ifdef LONG_TEST
    // Conversion to const should fail
    toTestValue.reset();
    conversionStatus = bdlmxxx::Convert::convert(&toConstObj, from);
    ASSERT_FAILED_CONVERSION;
#endif

    if (fromTypeEnum != ET::BDEM_VOID) {
        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::fromBdemType(&toObj, fromVoidPtr, fromTypeEnum);
        ASSERT_EXPECTED_RESULT;

#ifdef LONG_TEST
        // Conversion to const should fail
        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::fromBdemType(&toConstObj, fromVoidPtr, fromTypeEnum);
        ASSERT_FAILED_CONVERSION;
#endif
    }

    if (toTypeEnum != ET::BDEM_VOID) {
        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::toBdemType(toVoidPtr, toTypeEnum, from);
        ASSERT_EXPECTED_RESULT;

#ifdef LONG_TEST
        // Conversion to const should fail
        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::toBdemType(toConstVoidPtr, toTypeEnum, from);
        ASSERT_FAILED_CONVERSION;
#endif
    }

    if (fromTypeEnum != ET::BDEM_VOID && toTypeEnum != ET::BDEM_VOID) {
        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::convertBdemTypes(toVoidPtr, toTypeEnum,
                                           fromVoidPtr, fromTypeEnum);
        ASSERT_EXPECTED_RESULT;

#ifdef LONG_TEST
        // Conversion to const should fail
        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::convertBdemTypes(toConstVoidPtr, toTypeEnum,
                                           fromVoidPtr, fromTypeEnum);
        ASSERT_FAILED_CONVERSION;
#endif
    }

    // bdlmxxx::ElemRef conversions
    if (toTypeEnum != ET::BDEM_VOID) {
        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toElemRef, from);
        ASSERT_EXPECTED_RESULT;

        toTestValue.reset();
        toNullWord1 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toNullElemRef, from);
        if (expectedSuccess) {
            ASSERT_QUIET(!toNullElemRef.isNull());
        }
        else {
            ASSERT_QUIET(toNullElemRef.isNull());
        }

#ifdef LONG_TEST
        // Conversion to bdlmxxx::ConstElemRef should always fail.
        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toConstElemRef, from);
        ASSERT_FAILED_CONVERSION;

        toTestValue.reset();
        toNullWord1 = 1;
        ASSERT_QUIET(toNullConstElemRef.isNull());
        conversionStatus = bdlmxxx::Convert::convert(&toNullConstElemRef, from);
        ASSERT_FAILED_CONVERSION;
        if (expectedSuccess) {
            ASSERT_QUIET(!toNullConstElemRef.isNull());
        }
        else {
            ASSERT_QUIET(toNullConstElemRef.isNull());
        }
#endif
    }

    if (fromTypeEnum != ET::BDEM_VOID) {
        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toObj, fromElemRef);
        ASSERT_EXPECTED_RESULT;

        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toObj, fromConstElemRef);
        ASSERT_EXPECTED_RESULT;
    }

    if (fromTypeEnum != ET::BDEM_VOID && toTypeEnum != ET::BDEM_VOID) {
        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toElemRef, fromElemRef);
        ASSERT_EXPECTED_RESULT;
        ASSERT_QUIET(!toElemRef.isNull());

        toTestValue.reset();
        fromNullWord2 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toElemRef, fromNullElemRef);
        if (expectedSuccess) {
            ASSERT_QUIET(toElemRef.isNull());
        }
        else {
            ASSERT_QUIET(!toElemRef.isNull());
        }

        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toElemRef, fromConstElemRef);
        ASSERT_EXPECTED_RESULT;
        ASSERT(!toElemRef.isNull());

        toTestValue.reset();
        fromNullWord2 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toElemRef,
                                                 fromNullConstElemRef);
        if (expectedSuccess) {
            ASSERT_QUIET(toElemRef.isNull());
        }
        else {
            ASSERT_QUIET(!toElemRef.isNull());
        }

        toNullTestValue.reset();
        toNullWord1 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toNullElemRef, fromElemRef);
        ASSERT_NULL_EXPECTED_RESULT;
        if (expectedSuccess) {
            ASSERT_QUIET(!toNullElemRef.isNull());
        }
        else {
            ASSERT_QUIET(toNullElemRef.isNull());
        }

        toNullTestValue.reset();
        toNullWord1 = 1;
        fromNullWord2 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toNullElemRef,
                                                 fromNullElemRef);
        ASSERT(toNullElemRef.isNull());

        toNullTestValue.reset();
        toNullWord1 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toNullElemRef,
                                                 fromConstElemRef);
        ASSERT_NULL_EXPECTED_RESULT;
        if (expectedSuccess) {
            ASSERT_QUIET(!toNullElemRef.isNull());
        }
        else {
            ASSERT_QUIET(toNullElemRef.isNull());
        }

        toNullTestValue.reset();
        toNullWord1 = 1;
        fromNullWord2 = 1;
        conversionStatus = bdlmxxx::Convert::convert(&toNullElemRef,
                                                 fromNullConstElemRef);
        ASSERT_QUIET(toNullElemRef.isNull());

        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::toBdemType(toVoidPtr, toTypeEnum, fromElemRef);
        ASSERT_EXPECTED_RESULT;

        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::toBdemType(toVoidPtr, toTypeEnum, fromConstElemRef);
        ASSERT_EXPECTED_RESULT;

        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::fromBdemType(&toElemRef, fromVoidPtr, fromTypeEnum);
        ASSERT_EXPECTED_RESULT;

        toNullTestValue.reset();
        toNullWord1 = 1;
        conversionStatus = bdlmxxx::Convert::fromBdemType(&toNullElemRef,
                                                      fromVoidPtr,
                                                      fromTypeEnum);
        ASSERT_NULL_EXPECTED_RESULT;
        if (expectedSuccess) {
            ASSERT_QUIET(!toNullElemRef.isNull());
        }
        else {
            ASSERT_QUIET(toNullElemRef.isNull());
        }

#ifdef LONG_TEST
        // Conversions TO bdlmxxx::ConstElemRef should fail
        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toConstElemRef, fromElemRef);
        ASSERT_FAILED_CONVERSION;

        toTestValue.reset();
        conversionStatus = bdlmxxx::Convert::convert(&toConstElemRef,
                                                 fromConstElemRef);
        ASSERT_FAILED_CONVERSION;

        toTestValue.reset();
        conversionStatus =
            bdlmxxx::Convert::fromBdemType(&toConstElemRef, fromVoidPtr,
                                       fromTypeEnum);
        ASSERT_FAILED_CONVERSION;
#endif
#undef ASSERT_EXPECTED_RESULTS
#undef ASSERT_FAILED_CONVERSION
    }
}

#define TEST_GOOD_CONVERSIONS(line, initial, from, expectedResult) \
    testConversions((line), true, (initial), (from), (expectedResult))
#define TEST_BAD_CONVERSIONS(line, initial, from) \
    testConversions((line), false, (initial), (from), (initial))

template <class FROMTYPE>
void testNumericConv(const FROMTYPE& fromValue, bool isNull)
{
    const bool           initBoolF  = false;
    const bool           initBoolT  = true;
    const char           initChar   = 99;
    const short          initShort  = 99;
    const int            initInt    = 99;
    const Int64          initInt64  = 99;
    const float          initFloat  = 99;
    const double         initDouble = 99;
    const unsigned short initUShort = 99;
    const unsigned int   initUInt   = 99;
    const Uint64         initUInt64 = 99;
    const ConvertibleFrom<FROMTYPE> initCvtFt(FROMTYPE(99));
    const ConvertibleFrom<short>    initCvtS(initShort);
    const ConvertibleFrom<double>   initCvtD(initDouble);

    bool           expectedBool   = (bool) fromValue;
    char           expectedChar   = (char) fromValue;
    short          expectedShort  = (short) fromValue;
    int            expectedInt    = (int) fromValue;
    Int64          expectedInt64  = (Int64) fromValue;
    float          expectedFloat  = (float) fromValue;
    double         expectedDouble = (double) fromValue;
    unsigned short expectedUShort = (unsigned short) fromValue;
    unsigned int   expectedUInt   = (unsigned int) fromValue;
    Uint64         expectedUInt64 = (Uint64) fromValue;
    ConvertibleFrom<FROMTYPE> expectedCvtFt(fromValue);

    if (isNull) {
        bdltuxxx::Unset<bool>::makeUnset(&expectedBool);
        bdltuxxx::Unset<char>::makeUnset(&expectedChar);
        bdltuxxx::Unset<short>::makeUnset(&expectedShort);
        bdltuxxx::Unset<int>::makeUnset(&expectedInt);
        bdltuxxx::Unset<Int64>::makeUnset(&expectedInt64);
        bdltuxxx::Unset<float>::makeUnset(&expectedFloat);
        bdltuxxx::Unset<double>::makeUnset(&expectedDouble);
    }

    TEST_GOOD_CONVERSIONS(L_, initBoolF,  fromValue, expectedBool);
    TEST_GOOD_CONVERSIONS(L_, initBoolT,  fromValue, expectedBool);
    TEST_GOOD_CONVERSIONS(L_, initChar,   fromValue, expectedChar);
    TEST_GOOD_CONVERSIONS(L_, initShort,  fromValue, expectedShort);
    TEST_GOOD_CONVERSIONS(L_, initInt,    fromValue, expectedInt);
    TEST_GOOD_CONVERSIONS(L_, initInt64,  fromValue, expectedInt64);
    TEST_GOOD_CONVERSIONS(L_, initFloat,  fromValue, expectedFloat);
    TEST_GOOD_CONVERSIONS(L_, initDouble, fromValue, expectedDouble);
    TEST_GOOD_CONVERSIONS(L_, initUShort, fromValue, expectedUShort);
    TEST_GOOD_CONVERSIONS(L_, initUInt,   fromValue, expectedUInt);
    TEST_GOOD_CONVERSIONS(L_, initUInt64, fromValue, expectedUInt64);
    TEST_GOOD_CONVERSIONS(L_, initCvtFt,  fromValue, expectedCvtFt);
}

template <class FROMTYPE>
void testTimeConv(int                    line,
                  const FROMTYPE&        fromValue,
                  const bdlt::Date       *expD,
                  const bdlt::Time       *expT,
                  const bdlt::Datetime   *expDT,
                  const bdlt::DateTz     *expDZ,
                  const bdlt::TimeTz     *expTZ,
                  const bdlt::DatetimeTz *expDTZ)
{
    static const bdlt::Date initD(1980, 1, 1);
    static const bdlt::Time initT(6, 7, 8, 9);
    static const bdlt::Datetime initDT(initD, initT);
    static const bdlt::DateTz initDZ(initD, -6);
    static const bdlt::TimeTz initTZ(initT, -6);
    static const bdlt::DatetimeTz initDTZ(initDT, -6);

    testConversions(line, expD != 0,   initD,   fromValue, *expD);
    testConversions(line, expT != 0,   initT,   fromValue, *expT);
    testConversions(line, expDT != 0,  initDT,  fromValue, *expDT);
    testConversions(line, expDZ != 0,  initDZ,  fromValue, *expDZ);
    testConversions(line, expTZ != 0,  initTZ,  fromValue, *expTZ);
    testConversions(line, expDTZ != 0, initDTZ, fromValue, *expDTZ);

    FROMTYPE defFromType;
    const ConvertibleFrom<FROMTYPE> initTo(defFromType);
    const ConvertibleFrom<FROMTYPE> expTo(fromValue);
    TEST_GOOD_CONVERSIONS(line, initTo, fromValue, expTo);
}

template <class OTHER_TYPE>
void testStringConv(int               line,
                    const OTHER_TYPE& initOther,
                    const char*       strVal,
                    const OTHER_TYPE& other,
                    bool              toStr,
                    bool              fromStr)
{
    const bsl::string STRING_IV("Initial Value");

    if (toStr) {
        const bsl::string expStr(strVal);
        TEST_GOOD_CONVERSIONS(line, STRING_IV, other, expStr);
    }
    else {
        TEST_BAD_CONVERSIONS(line, STRING_IV, other);
    }

    static const char junk[] = "&^%$";
    static const int maxStrValLen = 80;
    int strLength = bsl::strlen(strVal);
    ASSERT(strLength <= maxStrValLen);

    // sizeof(junk) includes null terminator, extra byte for space
    char fromValAsCharArray[maxStrValLen + sizeof(junk) + 1];
    bsl::strcpy(fromValAsCharArray, strVal);
    char* fromValAsCharPtr = fromValAsCharArray;
    const char* fromValAsConstCharPtr = fromValAsCharArray;
    bsl::string fromValAsString(fromValAsCharArray);

    if (fromStr) {
        // Convert the string with no modifications
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsCharArray, other);
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsCharPtr, other);
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsConstCharPtr, other);
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsString, other);

        // Append junk with a space in-between
        fromValAsCharArray[strLength] = ' ';
        bsl::strcpy(fromValAsCharArray + strLength + 1, junk);
        fromValAsString = fromValAsCharArray;
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsCharArray, other);
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsCharPtr, other);
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsConstCharPtr, other);
        TEST_GOOD_CONVERSIONS(line, initOther, fromValAsString, other);

        // Append junk without a space in-between
        bsl::strcpy(fromValAsCharArray + strLength, junk);
        fromValAsString = fromValAsCharArray;
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsCharArray);
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsCharPtr);
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsConstCharPtr);
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsString);
    }
    else {
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsCharArray);
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsCharPtr);
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsConstCharPtr);
        TEST_BAD_CONVERSIONS(line, initOther, fromValAsString);
    }
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

#define USAGE_EXAMPLE 6
#if TEST_IS_ENABLED(USAGE_EXAMPLE)
// Assume we want to write a template function that takes a value and inserts
// it into a 'bdlmxxx::ElemRef', without knowing in advance what type the
// 'bdlmxxx::ElemRef' refers to and we want to honor conversions such as 'short'
// to 'int'.  The direct approach might start with a switch statement:
//..
//  template <typename VALTYPE>
//  void setValue(const bdlmxxx::ElemRef& elem, const VALTYPE& value) {
//      switch (elem.elemType()) {
//        case bdlmxxx::ElemType::BDEM_SHORT: {
//          elem.theModifiableShort()  = value;
//        } break;
//        case bdlmxxx::ElemType::BDEM_INT: {
//          elem.theModifiableInt()    = value;
//        } break;
//        case bdlmxxx::ElemType::BDEM_FLOAT: {
//          elem.theModifiableFloat()  = value;
//        } break;
//        case bdlmxxx::ElemType::BDEM_STRING: {
//          elem.theModifiableString() = value;
//        } break;
//        // etc.
//      }
//  }
//..
// The above code is not only tedious to write, but will not compile under
// most circumstances.  For example:
//..
//  setValue(elem, "hello");
//..
// Will fail to compile with a type mismatch in the first three cases because
// 'const char*' is not convertible to 'short', 'int', or 'float'.  Similarly,
//..
//  setValue(elem, 5.5);
//..
// Will fail to compile with a type mismatch on 'bdlmxxx::ElemType::BDEM_STRING'
// because 'float' is not is not convertible to 'bsl::string'.  Note that we
// do not even know what type 'elem' might hold -- even the branches that will
// not be taken at run-time must be valid at compile-time.
//
// The functions in this component solve this problem through sophisticated
// meta-programming.  Using these functions, we can re-write 'setValue' like
// this:
//..
    template <class VALTYPE>
    void setValue(const bdlmxxx::ElemRef& elem, const VALTYPE& value) {
        bdlmxxx::ElemType::Type elemType = elem.type();
        void *elemData = elem.data();
        if (bdlmxxx::Convert::toBdemType(elemData, elemType, value)) {
#ifdef BDE_BUILD_TARGET_EXC
            throw bsl::string("Invalid conversion");
#else
            return;
#endif // BDE_BUILD_TARGET_EXC
        }
    }
//..
// We can test this function with a simple main program.
//..
    int usageExample1() {
        int i = -1;
        float f = -2.2;
        bsl::string s = "junk";

        bdlmxxx::ElemRef iref(&i, &bdlmxxx::Properties::s_intAttr);
        bdlmxxx::ElemRef fref(&f, &bdlmxxx::Properties::s_floatAttr);
        bdlmxxx::ElemRef sref(&s, &bdlmxxx::Properties::s_stringAttr);

        setValue(iref, 9.8);
        ASSERT(9 == i);
        setValue(fref, 6);
        ASSERT(6.0 == f);
        setValue(sref, "Hello");
        ASSERT("Hello" == s);
//..
// This implementation can also take advantage of the automatic conversions to
// and from string types:
//..
        setValue(sref, 9.8);
        ASSERT("9.8" == s);
        setValue(iref, "123");
        ASSERT(123 == i);
//..
// To complete the picture, we should handle the errors.  This piece of code
// shows that a failed conversion does not modify the target value:
//..
#ifdef BDE_BUILD_TARGET_EXC
        try {
            setValue(sref, 4.6);   // successful
            setValue(iref, "xyz"); // "xyz" cannot be converted to int
            ASSERT("Shouldn't get here" == 0);
        }
        catch (const bsl::string& error) {
            ASSERT("Invalid conversion" == error);
            ASSERT("4.6" == s);  // Test that sref was changed
            ASSERT(123 == i);    // Test that iref was not changed
        }
#endif

        return 0;
    }
//..
#endif // TEST_IS_ENABLED(USAGE_EXAMPLE)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case USAGE_EXAMPLE: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns: The usage example in the header file compiles and runs.
        //
        // Plan: Copy the usage example from the header file.  Replace
        //   'assert' macro invocations with 'ASSERT'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------

#if TEST_IS_ENABLED(USAGE_EXAMPLE)
        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample1();

#endif
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING STRING CONVERSIONS
        //
        // Concerns:
        //   - 'bsl::string', 'char*', 'const char*', 'char []' and 'const
        //     char[]' are all convertible to 'bsl::string' with no data loss.
        //   - 'char*' is convertible to 'const char*', 'char[]' is
        //     convertible to 'char*' and 'const char*', etc.
        //   - 'bsl::string' is NOT convertible to 'char' pointer types.
        //   - All of the 'bdem' types are convertible to 'bsl::string'.
        //   - Any type that has an 'operator bsl::string' is convertible to
        //     'bsl::string'.
        //   - Any type that has 'bsl::ostream << type' is convertible to
        //     'bsl::string'.
        //   - All of the 'bdem' scalar types are convertible from
        //     'bsl::string'.
        //   - Adding extra words to the end of an input string does not change
        //     the value of the object created from that string.
        //   - Adding extra junk at the end of a string without an intervening
        //     space causes the conversion to fail.
        //
        // Plan:
        //   - Test all combinations of 'bsl::string' and 'char' pointer
        //     conversions using a simple input values.
        //   - Create an array of test vectors containing non-string values
        //     along with their string equivalents.  For each test vector,
        //     call a function that tests conversion of the non-string value
        //     to and from 'bsl::string'.  Each test vector indicates which
        //     conversions should succeed.
        //   - As orthogonal perturbations, junk is appended to the string
        //     value in each vector and conversion from that string is
        //     attempted again.  If the junk is separated from the original
        //     string with a space, then the conversion should still succeed.
        //     Otherwise, the conversion should fail.
        //
        // Testing:
        //     Conversions to and from 'bsl::string'
        // --------------------------------------------------------------------

#if TEST_IS_ENABLED(5)
        if (verbose) bsl::cout << "\nTESTING STRING CONVERSIONS"
                               << "\n==========================" << bsl::endl;

        if (verbose) bsl::cout << "Testing string-to-string conversions"
                               << bsl::endl;
        char              fromCharArray[] = "Hello world";
        const char        fromCCharArray[] = "Hello world";
        char       *const fromCharPtr = fromCharArray;
        const char *const fromCCharPtr = fromCCharArray;
        bsl::string       fromStr(fromCCharArray);

        char              initCharArray[] = "Garbage";
        const char        initCCharArray[] = "Garbage";
        char       *const initCharPtr = initCharArray;
        const char *const initCCharPtr = initCCharArray;
        bsl::string       initStr(fromCCharArray);

        //                   line Initial Value  Input Value     Expected Out
        //                   ---- -------------  --------------  ------------
        TEST_BAD_CONVERSIONS( L_, initCharArray, fromCharArray );
        TEST_BAD_CONVERSIONS( L_, initCharArray, fromCCharArray);
        TEST_BAD_CONVERSIONS( L_, initCharArray, fromCharPtr   );
        TEST_BAD_CONVERSIONS( L_, initCharArray, fromCCharPtr  );
        TEST_BAD_CONVERSIONS( L_, initCharArray, fromStr       );

        TEST_BAD_CONVERSIONS( L_, initCharPtr,   fromCharArray );
        TEST_BAD_CONVERSIONS( L_, initCharPtr,   fromCCharArray);
        TEST_GOOD_CONVERSIONS(L_, initCharPtr,   fromCharPtr,    fromCharPtr );
        TEST_BAD_CONVERSIONS( L_, initCharPtr,   fromCCharPtr  );
        TEST_BAD_CONVERSIONS( L_, initCharPtr,   fromStr       );

        TEST_GOOD_CONVERSIONS(L_, initCCharPtr,  fromCharArray,
                                                   (const char*) fromCharPtr );
        TEST_GOOD_CONVERSIONS(L_, initCCharPtr,  fromCCharArray,
                                                   (const char*) fromCCharPtr);
        TEST_GOOD_CONVERSIONS(L_, initCCharPtr,  fromCharPtr,
                                                  (const char*)  fromCharPtr );
        TEST_GOOD_CONVERSIONS(L_, initCCharPtr,  fromCCharPtr,   fromCCharPtr);
        TEST_BAD_CONVERSIONS( L_, initCCharPtr,  fromStr       );

        TEST_GOOD_CONVERSIONS(L_, initStr,       fromCharArray,  fromStr     );
        TEST_GOOD_CONVERSIONS(L_, initStr,       fromCCharArray, fromStr     );
        TEST_GOOD_CONVERSIONS(L_, initStr,       fromCharPtr,    fromStr     );
        TEST_GOOD_CONVERSIONS(L_, initStr,       fromCCharPtr,   fromStr     );
        TEST_GOOD_CONVERSIONS(L_, initStr,       fromStr,        fromStr     );
#endif // TEST_IS_ENABLED(5)
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DATE-TIME CONVERSIONS
        //
        // Concerns:
        //   Conversions between the date and time types will always fail
        //   except in two circumstances:
        //   - Each type will convert to itself, yielding a copy of the
        //     original.
        //   - 'bdlt::Date' is convertible to 'bdlt::Datetime'.  The resulting
        //     'bdlt::Datetime' object represents a date equal to the original
        //     date with a time of '0:00:00.000'.
        //   If the value being converted is a null value (as defined by
        //   'bdet::Unset<T>::isUnset'), then the result of the conversion is
        //   also a null value.
        //
        // Plan:
        //   For each of the bdem date and time types, call a templated
        //   function that attempts to convert a value of that type to each of
        //   the other date and time types.  Supply expected values for each
        //   successful conversion, or zero for an expected failed
        //   conversion.  The test vector supplies at least one null and at
        //   least one non-null input for each conversion.  Also, test that a
        //   class that is convertible to each date and time type can be
        //   converted to that type using 'bdlmxxx_convert' and that a class
        //   that is convertible from each date and time type can be
        //   converted from that type using 'bdlmxxx_convert'.
        //
        // Testing:
        //   Conversions between 'bdlt::Date', 'bdlt::Time', 'bdlt::Datetime',
        //   'bdlt::DateTz', 'bdlt::TimeTz', and 'bdlt::DatetimeTz'.
        // --------------------------------------------------------------------

#if TEST_IS_ENABLED(4)
        if (verbose) bsl::cout << "\nTESTING DATE-TIME CONVERSIONS"
                               << "\n============================="
                               << bsl::endl;

        static const bdlt::Date& DN = DATE_NULL;
        static const bdlt::Date& D1 = DATE_V1;
        static const bdlt::Date& D2 = DATE_V2;

        static const bdlt::Time& TN = TIME_NULL;
        static const bdlt::Time& T1 = TIME_V1;
        static const bdlt::Time& T2 = TIME_V2;

        static const bdlt::Datetime& DTN = DATETIME_NULL;
        static const bdlt::Datetime& DT1 = DATETIME_V1;
        static const bdlt::Datetime& DT2 = DATETIME_V2;
        static const bdlt::Datetime& DT3 = DATETIME_V3;
        static const bdlt::Datetime& DT4 = DATETIME_V4;

        static const bdlt::DateTz& DZN = DATETZ_NULL;
        static const bdlt::DateTz& DZ1 = DATETZ_V1;
        static const bdlt::DateTz& DZ2 = DATETZ_V2;
        static const bdlt::DateTz& DZ3 = DATETZ_V3;
        static const bdlt::DateTz& DZ4 = DATETZ_V4;

        static const bdlt::TimeTz& TZN = TIMETZ_NULL;
        static const bdlt::TimeTz& TZ1 = TIMETZ_V1;
        static const bdlt::TimeTz& TZ2 = TIMETZ_V2;
        static const bdlt::TimeTz& TZ3 = TIMETZ_V3;
        static const bdlt::TimeTz& TZ4 = TIMETZ_V4;

        static const bdlt::DatetimeTz& DTZN = DATETIMETZ_NULL;
        static const bdlt::DatetimeTz& DTZ1 = DATETIMETZ_V1;
        static const bdlt::DatetimeTz& DTZ2 = DATETIMETZ_V2;
        static const bdlt::DatetimeTz& DTZ3 = DATETIMETZ_V3;
        static const bdlt::DatetimeTz& DTZ4 = DATETIMETZ_V4;

        const ConvertibleTo<bdlt::Date> CD(D1);
        const ConvertibleTo<bdlt::Time> CT(T1);
        const ConvertibleTo<bdlt::Datetime> CDT(DT1);
        const ConvertibleTo<bdlt::DateTz> CDZ(DZ1);
        const ConvertibleTo<bdlt::TimeTz> CTZ(TZ1);
        const ConvertibleTo<bdlt::DatetimeTz> CDTZ(DTZ1);

        //          line input   date   time   dttm  dateTz timeTz dttmTz
        //          ---- ------ ------ ------ ------ ------ ------ ------
        testTimeConv(L_,    DN,   &DN,     0,  &DTN,  &DZN,     0,     0);
        testTimeConv(L_,    D1,   &D1,     0,  &DT3,  &DZ3,     0,     0);
        testTimeConv(L_,    D2,   &D2,     0,  &DT4,  &DZ4,     0,     0);

        testTimeConv(L_,    TN,     0,   &TN,     0,     0,  &TZN,     0);
        testTimeConv(L_,    T1,     0,   &T1,     0,     0,  &TZ3,     0);
        testTimeConv(L_,    T2,     0,   &T2,     0,     0,  &TZ4,     0);

        testTimeConv(L_,   DTN,     0,     0,  &DTN,     0,     0, &DTZN);
        testTimeConv(L_,   DT1,     0,     0,  &DT1,     0,     0, &DTZ3);
        testTimeConv(L_,   DT2,     0,     0,  &DT2,     0,     0, &DTZ4);

        testTimeConv(L_,   DZN,     0,     0,     0,  &DZN,     0,     0);
        testTimeConv(L_,   DZ1,     0,     0,     0,  &DZ1,     0,     0);
        testTimeConv(L_,   DZ2,     0,     0,     0,  &DZ2,     0,     0);

        testTimeConv(L_,   TZN,     0,     0,     0,     0,  &TZN,     0);
        testTimeConv(L_,   TZ1,     0,     0,     0,     0,  &TZ1,     0);
        testTimeConv(L_,   TZ2,     0,     0,     0,     0,  &TZ2,     0);

        testTimeConv(L_,  DTZN,     0,     0,     0,     0,     0, &DTZN);
        testTimeConv(L_,  DTZ1,     0,     0,     0,     0,     0, &DTZ1);
        testTimeConv(L_,  DTZ2,     0,     0,     0,     0,     0, &DTZ2);

        testTimeConv(L_,    CD,   &D1,     0,     0,     0,     0,     0);
        testTimeConv(L_,    CT,     0,   &T1,     0,     0,     0,     0);
        testTimeConv(L_,   CDT,     0,     0,  &DT1,     0,     0,     0);
        testTimeConv(L_,   CDZ,     0,     0,     0,  &DZ1,     0,     0);
        testTimeConv(L_,   CTZ,     0,     0,     0,     0,  &TZ1,     0);
        testTimeConv(L_,  CDTZ,     0,     0,     0,     0,     0, &DTZ1);
#endif // TEST_IS_ENABLED(4)
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING NUMERIC CONVERSIONS
        //
        // Concerns:
        //   'bdlmxxx::Convert' methods will always succeed at converting from a
        //   bdem numeric type to any other bdem numeric type and will produce
        //   the same result as normal C++ assignment except in the case of
        //   null values.  If the type being converted is not 'bool' or 'char'
        //   and if the value being converted is a null value (as defined by
        //   'bdet::Unset<T>::isUnset'), then the result of the conversion is
        //   also a null value.  Conversions from 'bool' or 'char' no not
        //   follow special rules for null values.  Conversions to and from
        //   unsigned types (which do not have corresponding bdem enumeration
        //   constants) also work, although the null rules are ignored.
        //
        // Plan:
        //   For each bdem numeric type as well as 'unsigned short', 'unsigned
        //   int', and 'Uint64', call a templated function that
        //   converts a value of that type to each of the other numeric
        //   types.  The test vector supplies the zero value, null value,
        //   and a few other values for each numeric type.  This will result
        //   in complete coverage of the bdem numeric types and enough
        //   coverage of non-bdem numeric types to give us reasonable
        //   confidence in the implementation.
        //
        // Testing:
        //   Numeric conversions
        // --------------------------------------------------------------------

#if TEST_IS_ENABLED(3)
        if (verbose) bsl::cout << "\nTESTING NUMERIC CONVERSIONS"
                               << "\n===========================" << bsl::endl;

        const ConvertibleTo<float> CONV_TOFLOAT(FLOAT_V1);

        //              Input Value  Is Null
        //              ===========  =======
        testNumericConv(BOOL_ZERO,   false);
        testNumericConv(BOOL_V1,     false);

        testNumericConv(CHAR_ZERO,   false);
        testNumericConv(CHAR_V1,     false);
        testNumericConv(CHAR_V2,     false);

        testNumericConv(SHORT_NULL,  true);
        testNumericConv(SHORT_ZERO,  false);
        testNumericConv(SHORT_V1,    false);
        testNumericConv(SHORT_V2,    false);

        testNumericConv(INT_NULL,    true);
        testNumericConv(INT_ZERO,    false);
        testNumericConv(INT_V1,      false);
        testNumericConv(INT_V2,      false);

        testNumericConv(INT64_NULL,  true);
        testNumericConv(INT64_ZERO,  false);
        testNumericConv(INT64_V1,    false);
        testNumericConv(INT64_V2,    false);

        testNumericConv(FLOAT_NULL,  true);
        testNumericConv(FLOAT_ZERO,  false);
        testNumericConv(FLOAT_V1,    false);
        testNumericConv(FLOAT_V2,    false);

        testNumericConv(DOUBLE_NULL, true);
        testNumericConv(DOUBLE_ZERO, false);
        testNumericConv(DOUBLE_V1,   false);
        testNumericConv(DOUBLE_V2,   false);

        testNumericConv(USHORT_ZERO, false);
        testNumericConv(USHORT_V1,   false);
        testNumericConv(USHORT_V2,   false);

        testNumericConv(UINT_ZERO,   false);
        testNumericConv(UINT_V1,     false);
        testNumericConv(UINT_V2,     false);

        testNumericConv(UINT64_ZERO, false);
        testNumericConv(UINT64_V1,   false);
        testNumericConv(UINT64_V2,   false);

        testNumericConv(CONV_TOFLOAT,false);

#endif // TEST_IS_ENABLED(3)
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //   The template functions that test the methods in 'bdlmxxx::Convert'
        //   correctly identify when the methods do or do not produce the
        //   expected result (where the expected call is provided to the
        //   function or is implied by the expectation of failure).
        //   Conversion tests of types to, from, and between non-bdem types
        //   also works, although fewer 'bdlmxxx::Convert' methods are tested in
        //   those cases.
        //
        // Plan:
        //   Call each test function with parameters where the expected result
        //   matches the actual result.  Verify that the tests do not report
        //   errors (i.e. the 'testStatus' variable is not incremented).
        //   Also call each test function with incorrect expected results and
        //   verify that the 'testStatus' value increases.  The latter test
        //   requires that 'testStatus' be saved and then restored, so that a
        //   "successful failure" test does not result in the test driver
        //   reporting failure.
        //
        // Testing:
        //   void TEST_GOOD_CONVERSIONS(int             line,
        //                            const TOTYPE&   initial,
        //                            const FROMTYPE& from,
        //                            const TOTYPE&   expectedResult);
        //   void TEST_BAD_CONVERSIONS(int             line,
        //                           const TOTYPE&   initial,
        //                           const FROMTYPE& from);
        // --------------------------------------------------------------------

#if TEST_IS_ENABLED(2)
        if (verbose) bsl::cout << "\nTESTING TEST APPARATUS"
                               << "\n======================" << bsl::endl;

        double   d = -3.6;
        int      i1 = 99, i2 = -3;
        unsigned u1 = 999, u2 = unsigned(-3);

        bdlt::Date date1(1776, 7, 4), date2(2006, 3, 20);

        if (veryVerbose) bsl::cout << "Testing successful calls" << bsl::endl;
        TEST_GOOD_CONVERSIONS(L_, i1, d, i2);
        TEST_GOOD_CONVERSIONS(L_, i1, u2, i2);
        TEST_GOOD_CONVERSIONS(L_, u1, i2, u2);
        TEST_GOOD_CONVERSIONS(L_, u1, u2, u2);
        TEST_BAD_CONVERSIONS(L_, date1, d);
        TEST_BAD_CONVERSIONS(L_, date1, u1);
        TEST_BAD_CONVERSIONS(L_, u1, date1);

        if (veryVerbose) bsl::cout << "Testing failed calls" << bsl::endl;

        bsl::streambuf *saveCout = bsl::cout.rdbuf();
        if (veryVeryVerbose) {
            bsl::cout << "*** TESTING BAD CONVERSIONS.  THE FOLLOWING ERRORS "
                      << "ARE DELIBERATE: ***" << bsl::endl;
        }

        int saveStatus = testStatus;

        TEST_GOOD_CONVERSIONS(L_, date1, d, date2);
        int newStatus1 = testStatus;

        TEST_BAD_CONVERSIONS(L_, i1, d);
        int newStatus2 = testStatus;

        TEST_BAD_CONVERSIONS(L_, i1, u2);
        int newStatus3 = testStatus;
        TEST_BAD_CONVERSIONS(L_, u1, i2);
        int newStatus4 = testStatus;
        TEST_BAD_CONVERSIONS(L_, u1, u2);
        int newStatus5 = testStatus;

        bsl::cout.rdbuf(saveCout);  // Restore cout
        testStatus = saveStatus;  // restore status
        if (veryVeryVerbose) bsl::cout << "*** END DELIBERATE ERRORS ***"
                                       << bsl::endl;

        static const int expectedFailuresPerMethod = 1;
        LOOP2_ASSERT(saveStatus, newStatus1,
                     newStatus1 >= saveStatus + expectedFailuresPerMethod * 4);
        LOOP2_ASSERT(newStatus1, newStatus2,
                     newStatus2 >= newStatus1 + expectedFailuresPerMethod * 4);
        LOOP2_ASSERT(newStatus2, newStatus3,
                     newStatus3 >= newStatus2 + expectedFailuresPerMethod * 2);
        LOOP2_ASSERT(newStatus3, newStatus4,
                     newStatus4 >= newStatus3 + expectedFailuresPerMethod * 2);
        LOOP2_ASSERT(newStatus4, newStatus5,
                     newStatus5 >= newStatus4 + expectedFailuresPerMethod * 1);

#endif // TEST_IS_ENABLED(2)
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Every method in 'bdlmxxx::Convert' can be instantiated and returns
        //   correct results.
        //
        // Plan:
        //   Call each method with a variety of input types and values.
        //   Verify that the expected status and converted values from each
        //   call verify expectations.  Probe a sampling of special cases.
        //
        // Tactics:
        //   Ad-hoc data selection.
        //
        // Exercises:
        //   static int convert(TOTYPE *toAddr, const FROMTYPE& fromVal);
        //   static int toBdemType(void                *toAddr,
        //                         bdlmxxx::ElemType::Type  toType,
        //                         const FROMTYPE&      fromVal);
        //   static int fromBdemType(TOTYPE              *toAddr,
        //                           const void          *fromAddr,
        //                           bdlmxxx::ElemType::Type  fromType);
        //   static int convertBdemTypes(void                *toAddr,
        //                               bdlmxxx::ElemType::Type  toType,
        //                               const void          *fromAddr,
        //                               bdlmxxx::ElemType::Type  fromType);
        // --------------------------------------------------------------------

#if TEST_IS_ENABLED(1)
        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        const char  valChar = 'A';
        const short nullShort = bdltuxxx::Unset<short>::unsetValue();
        const int   nullInt = bdltuxxx::Unset<int>::unsetValue();
        const float valFloat = 232.3;
        const bsl::string valString("123.4 hello");
        const bsl::string valDatetimeTzString("1996-04-05T09:30:00.000-04:00");
        const bsl::string valIntVecString("[ 3 2 1 ]");
        const bdlt::Date valDate(2006, 2, 14);
        const bdlt::Time valTime(11, 23);
        const bdlt::Datetime valDatetime(valDate, valTime);
        const bool valBool = true;
        const bdlt::DateTz valDateTz(bdlt::Date(2000, 5, 6), -5 * 60);
        const bdlt::TimeTz valTimeTz(bdlt::Time(10, 56), -5 * 60);
        const bdlt::DatetimeTz
            valDatetimeTz(bdlt::Datetime(bdlt::Date(1996, 4, 5),
                                        bdlt::Time(9, 30)), -4 * 60);
        static int vecData[] = { 3, 2, 1 };
        const bsl::vector<int> valIntVec(vecData, vecData + 3);

        int status;

        if (verbose) bsl::cout << "Testing self conversion" << bsl::endl;
        char xChar = 0;
        status = bdlmxxx::Convert::convert(&xChar, valChar);
        ASSERT(0 == status);
        ASSERT(valChar == xChar);

        bdlt::DatetimeTz xDatetimeTz;
        status = bdlmxxx::Convert::convert(&xDatetimeTz, valDatetimeTz);
        ASSERT(0 == status);
        ASSERT(valDatetimeTz == xDatetimeTz);

        if (verbose) bsl::cout << "Testing conversion to int" << bsl::endl;
        int xInt = 0;
        status = bdlmxxx::Convert::convert(&xInt, valChar);
        ASSERT(0 == status && 'A' == xInt);

        xInt = 0;
        status = bdlmxxx::Convert::convert(&xInt, valString);
        ASSERT(0 == status && 123 == xInt);

        xChar = 0;
        status = bdlmxxx::Convert::convert(&xChar, valString);
        ASSERT(0 == status && '1' == xChar);

        xInt = 0;
        status = bdlmxxx::Convert::convert(&xInt, valDate);
        ASSERT(0 != status && 0 == xInt);

        xInt = 0;
        status = bdlmxxx::Convert::convert(&xInt, nullShort);
        ASSERT(0 == status && nullInt == xInt);

        if (verbose) bsl::cout << "Testing conversion to string" << bsl::endl;
        bsl::string xString;
        status = bdlmxxx::Convert::convert(&xString, valFloat);
        LOOP_ASSERT(xString, 0 == status && "232.3" == xString);

        status = bdlmxxx::Convert::convert(&xString, nullShort);
        LOOP_ASSERT(xString, 0 == status && "" == xString);

        status = bdlmxxx::Convert::convert(&xString, valDatetimeTz);
        LOOP_ASSERT(xString, 0 == status && valDatetimeTzString == xString);

        status = bdlmxxx::Convert::convert(&xString, valIntVec);
        LOOP_ASSERT(xString, 0 == status && valIntVecString == xString);

        if (verbose) bsl::cout << "Testing conversion from string"
                               << bsl::endl;
        xDatetimeTz = bdlt::DatetimeTz();
        status = bdlmxxx::Convert::convert(&xDatetimeTz, valDatetimeTzString);
        LOOP_ASSERT(xDatetimeTz, 0 == status && valDatetimeTz == xDatetimeTz);

        xDatetimeTz = bdlt::DatetimeTz();
        status = bdlmxxx::Convert::convert(&xDatetimeTz,
                                       valDatetimeTzString.c_str());
        LOOP_ASSERT(xDatetimeTz, 0 == status && valDatetimeTz == xDatetimeTz);

        bsl::vector<int> xIntVec;
        status = bdlmxxx::Convert::convert(&xIntVec, valIntVecString);
        ASSERT(0 != status && xIntVec.empty());

        if (verbose) bsl::cout << "Testing toBdemType" << bsl::endl;
        bool yBool = false;
        void* p = &yBool;
        status = bdlmxxx::Convert::toBdemType(p, bdlmxxx::ElemType::BDEM_BOOL, "A");
        ASSERT(0 != status && false == yBool);

        char TrueString[] = "True";
        status = bdlmxxx::Convert::toBdemType(p, bdlmxxx::ElemType::BDEM_BOOL,
                                          TrueString);
        ASSERT(0 == status && true == yBool);

        short yShort = -1;
        p = &yShort;
        status = bdlmxxx::Convert::toBdemType(p, bdlmxxx::ElemType::BDEM_SHORT,
                                          valBool);
        ASSERT(0 == status && 1 == yShort);

        status = bdlmxxx::Convert::toBdemType(p, bdlmxxx::ElemType::BDEM_SHORT,
                                          valDate);
        ASSERT(0 != status && 1 == yShort);

        if (verbose) bsl::cout << "Testing fromBdemType" << bsl::endl;
        bdlt::Time yTime;
        const void* cp = &valTime;
        status = bdlmxxx::Convert::fromBdemType(&yTime, cp,
                                            bdlmxxx::ElemType::BDEM_TIME);
        ASSERT(0 == status && valTime == yTime);

        double yDouble = 0.0;
        const bsl::string emptyStr("");
        cp = &emptyStr;
        status = bdlmxxx::Convert::fromBdemType(&yDouble, cp,
                                            bdlmxxx::ElemType::BDEM_STRING);
        ASSERT(0 == status && bdltuxxx::Unset<double>::isUnset(yDouble));

        cp = &valString;
        status = bdlmxxx::Convert::fromBdemType(&yDouble, cp,
                                            bdlmxxx::ElemType::BDEM_STRING);
        ASSERT(0 == status && 123.4 == yDouble);

        bsl::string yString;
        status = bdlmxxx::Convert::fromBdemType(&yString, cp,
                                            bdlmxxx::ElemType::BDEM_STRING);
        ASSERT(0 == status && "123.4 hello" == yString);
#endif // TEST_IS_ENABLED(1)
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE MEASUREMENT
        //
        // Concerns:
        //   Evaluate the speed of conversions between basic fundamental types,
        //   date/time types, and strings.
        //
        // Plan:
        //   Time the conversions in a loop, repeated so that the total time is
        //   at most 1s (or the floating point value in seconds passed as
        //   second argument on the command line, after the test case number
        //   -1).
        //
        // Tactics:
        //   Exhaustive
        //
        // Exercises:
        //   static int convert(TOTYPE *toAddr, const FROMTYPE& fromVal);
        //   static int toBdemType(void                *toAddr,
        //                         bdlmxxx::ElemType::Type  toType,
        //                         const FROMTYPE&      fromVal);
        //   static int fromBdemType(TOTYPE              *toAddr,
        //                           const void          *fromAddr,
        //                           bdlmxxx::ElemType::Type  fromType);
        //   static int convertBdemTypes(void                *toAddr,
        //                               bdlmxxx::ElemType::Type  toType,
        //                               const void          *fromAddr,
        //                               bdlmxxx::ElemType::Type  fromType);
        // --------------------------------------------------------------------

// TBD This test driver is large enough as it is without also enabling
// compilation of this singularly large negative test.  The inclusion of this
// case causes CC/Studio-12/64-bit compilation to fail with:
//..
//  Insufficient memory is available for compilation to continue.
//..
// Given the enormity of it all, and the fact that the performance concerns
// of this component are minimal, this case is commented out for *all*
// platforms.

#if 0

#if TEST_IS_ENABLED(-1)
        if (verbose) bsl::cout << "\nPERFORMANCE TEST"
                               << "\n================" << bsl::endl;

        enum { NUM_ITER = 10000 };
        double limit = argc > 2 ? bsl::atof(argv[2]) : 1.;  // 1s

        const char          valChar = 'A';
        const short         nullShort = bdltuxxx::Unset<short>::unsetValue();
        const short         valShort = -32768;
        const int           nullInt = bdltuxxx::Unset<int>::unsetValue();
        const int           valInt = -2147483648LL;
        const Int64         nullInt64 = bdltuxxx::Unset<Int64>::unsetValue();
        const Int64         valInt64 = -9223372036854775808LL;
        const float         valFloat = 123456789.1234567989;
        const double        valDouble = 123456789.1234567989;
        const long double   valLDouble = 123456789.1234567989;
        const bsl::string   nullString;
        const bsl::string   valString("1996-04-05T09:30:00.000-04:00");
        const bdlt::Date     valDate(2006, 2, 14);
        const bdlt::Time     valTime(11, 23);
        const bdlt::Datetime valDatetime(valDate, valTime);
        const bdlt::DateTz   valDateTz(bdlt::Date(2000, 5, 6), -5 * 60);
        const bdlt::TimeTz   valTimeTz(bdlt::Time(10, 56), -5 * 60);
        const bdlt::DatetimeTz
                            valDatetimeTz(bdlt::Datetime(bdlt::Date(1996, 4, 5),
                                          bdlt::Time(9, 30)), -4 * 60);

        char            xChar = 0;
        short           xShort = 0;
        int             xInt = 0;
        Int64           xInt64 = 0;
        float           xFloat = 0;
        double          xDouble = 0;
        long double     xLDouble = 0;
        bsl::string     xString;
        bdlt::Date       xDate;
        bdlt::Time       xTime;
        bdlt::Datetime   xDatetime;
        bdlt::DateTz     xDateTz;
        bdlt::TimeTz     xTimeTz;
        bdlt::DatetimeTz xDatetimeTz;

        if (verbose) bsl::cout << "Testing convert.\n"
                               << "----------------" << bsl::endl;

#define TIME_FROM_TO(FROM_TYPE, FROM_STR, TO_TYPE, TO_STR, NULL_OR_VAL) {    \
        bsls::Stopwatch timer;                                               \
        int iters = NUM_ITER / 2;                                            \
        int status = bdlmxxx::Convert::convert(&x##TO_TYPE,                      \
                                           NULL_OR_VAL##FROM_TYPE);          \
        int status2 = status;                                                \
        do {                                                                 \
            timer.reset();                                                   \
            iters *= 2;                                                      \
            timer.start();                                                   \
            for (int i = 0; i < iters; ++i) {                                \
                status += bdlmxxx::Convert::convert(&x##TO_TYPE,                 \
                                                NULL_OR_VAL##FROM_TYPE);     \
            }                                                                \
            timer.stop();                                                    \
        } while (timer.elapsedTime() < limit);                               \
        ASSERT((0 == status && 0 == status2) || status < status2);           \
        bsl::cout << "\t" << iters << " conversions"                         \
                  << " from " << FROM_STR << " to " << TO_STR << " in "      \
                  << timer.elapsedTime() << " seconds ("                     \
                  << timer.elapsedTime() / iters << " per conversion)."      \
                  << bsl::endl;                                              \
    }
        // This macro evaluates the 'bdlmxxx::Convert::convert' operation applied
        // from a variable of parameterized 'FROM_TYPE' to a modifiable
        // reference to an instance of the parameterized 'TO_TYPE', repeatedly
        // so that the total running time exceeds 1s (or the limit passed as
        // second argument after the test case number -1 on the command line),
        // and reports to 'bsl::cout' the statistics of the timing.  The
        // instance of 'FROM_TYPE' is null if the parameter 'NULL_OR_VAL' is
        // "null", otherwise 'NV' should be passed 'val' for some (unspecified)
        // value of 'FROM_TYPE'.

        bsl::cout << "Testing conversion to char." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        Char, "char", val)
        TIME_FROM_TO(Short,      "short",       Char, "char", val)
        TIME_FROM_TO(Short,      "null short",  Char, "char", null)
        TIME_FROM_TO(Int,        "int",         Char, "char", val)
        TIME_FROM_TO(Int,        "null int",    Char, "char", null)
        TIME_FROM_TO(Int64,      "int64",       Char, "char", val)
        TIME_FROM_TO(Int64,      "null int64",  Char, "char", null)
        TIME_FROM_TO(Float,      "float",       Char, "char", val)
        TIME_FROM_TO(Double,     "double",      Char, "char", val)
        TIME_FROM_TO(LDouble,    "long double", Char, "char", val)
        TIME_FROM_TO(String,     "string",      Char, "char", val)
        TIME_FROM_TO(String,     "null string", Char, "char", null)

        bsl::cout << "Testing conversion to short." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        Short, "short", val)
        TIME_FROM_TO(Short,      "short",       Short, "short", val)
        TIME_FROM_TO(Short,      "null short",  Short, "short", null)
        TIME_FROM_TO(Int,        "int",         Short, "short", val)
        TIME_FROM_TO(Int,        "null int",    Short, "short", null)
        TIME_FROM_TO(Int64,      "int64",       Short, "short", val)
        TIME_FROM_TO(Int64,      "null int64",  Short, "short", null)
        TIME_FROM_TO(Float,      "float",       Short, "short", val)
        TIME_FROM_TO(Double,     "double",      Short, "short", val)
        TIME_FROM_TO(LDouble,    "long double", Short, "short", val)
        TIME_FROM_TO(String,     "string",      Short, "short", val)
        TIME_FROM_TO(String,     "null string", Short, "short", null)

        bsl::cout << "Testing conversion to int." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        Int, "int", val)
        TIME_FROM_TO(Short,      "short",       Int, "int", val)
        TIME_FROM_TO(Short,      "null short",  Int, "int", null)
        TIME_FROM_TO(Int,        "int",         Int, "int", val)
        TIME_FROM_TO(Int,        "null int",    Int, "int", null)
        TIME_FROM_TO(Int64,      "int64",       Int, "int", val)
        TIME_FROM_TO(Int64,      "null int64",  Int, "int", null)
        TIME_FROM_TO(Float,      "float",       Int, "int", val)
        TIME_FROM_TO(Double,     "double",      Int, "int", val)
        TIME_FROM_TO(LDouble,    "long double", Int, "int", val)
        TIME_FROM_TO(String,     "string",      Int, "int", val)
        TIME_FROM_TO(String,     "null string", Int, "int", null)

        bsl::cout << "Testing conversion to Int64." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        Int64, "int64", val)
        TIME_FROM_TO(Short,      "short",       Int64, "int64", val)
        TIME_FROM_TO(Short,      "null short",  Int64, "int64", null)
        TIME_FROM_TO(Int,        "int",         Int64, "int64", val)
        TIME_FROM_TO(Int,        "null int",    Int64, "int64", null)
        TIME_FROM_TO(Int64,      "int64",       Int64, "int64", val)
        TIME_FROM_TO(Int64,      "null int64",  Int64, "int64", null)
        TIME_FROM_TO(Float,      "float",       Int64, "int64", val)
        TIME_FROM_TO(Double,     "double",      Int64, "int64", val)
        TIME_FROM_TO(LDouble,    "long double", Int64, "int64", val)
        TIME_FROM_TO(String,     "string",      Int64, "int64", val)
        TIME_FROM_TO(String,     "null string", Int64, "int64", null)

        bsl::cout << "Testing conversion to float." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        Float, "float", val)
        TIME_FROM_TO(Short,      "short",       Float, "float", val)
        TIME_FROM_TO(Short,      "null short",  Float, "float", null)
        TIME_FROM_TO(Int,        "int",         Float, "float", val)
        TIME_FROM_TO(Int,        "null int",    Float, "float", null)
        TIME_FROM_TO(Int64,      "int64",       Float, "float", val)
        TIME_FROM_TO(Int64,      "null int64",  Float, "float", null)
        TIME_FROM_TO(Float,      "float",       Float, "float", val)
        TIME_FROM_TO(Double,     "double",      Float, "float", val)
        TIME_FROM_TO(LDouble,    "long double", Float, "float", val)
        TIME_FROM_TO(String,     "string",      Float, "float", val)
        TIME_FROM_TO(String,     "null string", Float, "float", null)

        bsl::cout << "Testing conversion to double." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        Double, "double", val)
        TIME_FROM_TO(Short,      "short",       Double, "double", val)
        TIME_FROM_TO(Short,      "null short",  Double, "double", null)
        TIME_FROM_TO(Int,        "int",         Double, "double", val)
        TIME_FROM_TO(Int,        "null int",    Double, "double", null)
        TIME_FROM_TO(Int64,      "int64",       Double, "double", val)
        TIME_FROM_TO(Int64,      "null int64",  Double, "double", null)
        TIME_FROM_TO(Float,      "float",       Double, "double", val)
        TIME_FROM_TO(Double,     "double",      Double, "double", val)
        TIME_FROM_TO(LDouble,    "long double", Double, "double", val)
        TIME_FROM_TO(String,     "string",      Double, "double", val)
        TIME_FROM_TO(String,     "null string", Double, "double", null)

        bsl::cout << "Testing conversion to long double." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        LDouble, "long double", val)
        TIME_FROM_TO(Short,      "short",       LDouble, "long double", val)
        TIME_FROM_TO(Short,      "null short",  LDouble, "long double", null)
        TIME_FROM_TO(Int,        "int",         LDouble, "long double", val)
        TIME_FROM_TO(Int,        "null int",    LDouble, "long double", null)
        TIME_FROM_TO(Int64,      "int64",       LDouble, "long double", val)
        TIME_FROM_TO(Int64,      "null int64",  LDouble, "long double", null)
        TIME_FROM_TO(Float,      "float",       LDouble, "long double", val)
        TIME_FROM_TO(Double,     "double",      LDouble, "long double", val)
        TIME_FROM_TO(LDouble,    "long double", LDouble, "long double", val)
        TIME_FROM_TO(String,     "string",      LDouble, "long double", val)
        TIME_FROM_TO(String,     "null string", LDouble, "long double", null)

        bsl::cout << "Testing conversion to string" << bsl::endl;

        TIME_FROM_TO(Char,       "char",        String, "string", val)
        TIME_FROM_TO(Short,      "short",       String, "string", val)
        TIME_FROM_TO(Short,      "null short",  String, "string", null)
        TIME_FROM_TO(Int,        "int",         String, "string", val)
        TIME_FROM_TO(Int,        "null int",    String, "string", null)
        TIME_FROM_TO(Int64,      "int64",       String, "string", val)
        TIME_FROM_TO(Int64,      "null int64",  String, "string", null)
        TIME_FROM_TO(Float,      "float",       String, "string", val)
        TIME_FROM_TO(Double,     "double",      String, "string", val)
        TIME_FROM_TO(LDouble,    "long double", String, "string", val)
        TIME_FROM_TO(String,     "string",      String, "string", val)
        TIME_FROM_TO(String,     "null string", String, "string", null)
        TIME_FROM_TO(Date,       "date",        String, "string", val)
        TIME_FROM_TO(Time,       "time",        String, "string", val)
        TIME_FROM_TO(Datetime,   "datetime",    String, "string", val)
        TIME_FROM_TO(DateTz,     "dateTz",      String, "string", val)
        TIME_FROM_TO(TimeTz,     "timeTz",      String, "string", val)
        TIME_FROM_TO(DatetimeTz, "datetimeTz",  String, "string", val)

#undef TIME_FROM_TO

        if (verbose) bsl::cout << "Testing toBdemType.\n"
                               << "-------------------" << bsl::endl;

#define TIME_FROM_TO(FROM_TYPE, FROM_STR, TO_BDEM, TO_TYPE, TO_STR) {        \
        bsls::Stopwatch timer;                                               \
        int iters = NUM_ITER / 2;                                            \
        int status = bdlmxxx::Convert::toBdemType(&x##TO_TYPE,                   \
                                              bdlmxxx::ElemType::TO_BDEM,        \
                                              val##FROM_TYPE);               \
        int status2 = status;                                                \
        do {                                                                 \
            timer.reset();                                                   \
            iters *= 2;                                                      \
            timer.start();                                                   \
            for (int i = 0; i < iters; ++i) {                                \
                status += bdlmxxx::Convert::toBdemType(&x##TO_TYPE,              \
                                                   bdlmxxx::ElemType::TO_BDEM,   \
                                                   val##FROM_TYPE);          \
            }                                                                \
            timer.stop();                                                    \
        } while (timer.elapsedTime() < limit);                               \
        ASSERT((0 == status && 0 == status2) || status < status2);           \
        bsl::cout << "\t" << iters << " conversions"                         \
                  << " from " << FROM_STR << " to " << TO_STR << " [bdem]"   \
                  << " in " << timer.elapsedTime() << " seconds ("           \
                  << timer.elapsedTime() / iters << " per conversion)."      \
                  << bsl::endl;                                              \
    }
        // This macro evaluates the 'bdlmxxx::Convert::convert' operation applied
        // from a variable of parameterized 'FROM_TYPE' to a modifiable 'bdem'
        // reference to an instance of the parameterized 'TO_TYPE', repeatedly
        // so that the total running time exceeds 1s (or the limit passed as
        // second argument after the test case number -1 on the command line),
        // and reports to 'bsl::cout' the statistics of the timing.  The
        // instance of 'FROM_TYPE' is null if the parameter 'NULL_OR_VAL' is
        // "null", otherwise 'NV' should be passed 'val' for some (unspecified)
        // value of 'FROM_TYPE'.

        bsl::cout << "Testing conversion to char." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_CHAR, Char, "char")
        TIME_FROM_TO(Short,      "short",       BDEM_CHAR, Char, "char")
        TIME_FROM_TO(Int,        "int",         BDEM_CHAR, Char, "char")
        TIME_FROM_TO(Int64,      "int64",       BDEM_CHAR, Char, "char")
        TIME_FROM_TO(Float,      "float",       BDEM_CHAR, Char, "char")
        TIME_FROM_TO(Double,     "double",      BDEM_CHAR, Char, "char")
        TIME_FROM_TO(LDouble,    "long double", BDEM_CHAR, Char, "char")
        TIME_FROM_TO(String,     "string",      BDEM_CHAR, Char, "char")

        bsl::cout << "Testing conversion to short." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_SHORT, Short, "short")
        TIME_FROM_TO(Short,      "short",       BDEM_SHORT, Short, "short")
        TIME_FROM_TO(Int,        "int",         BDEM_SHORT, Short, "short")
        TIME_FROM_TO(Int64,      "int64",       BDEM_SHORT, Short, "short")
        TIME_FROM_TO(Float,      "float",       BDEM_SHORT, Short, "short")
        TIME_FROM_TO(Double,     "double",      BDEM_SHORT, Short, "short")
        TIME_FROM_TO(LDouble,    "long double", BDEM_SHORT, Short, "short")
        TIME_FROM_TO(String,     "string",      BDEM_SHORT, Short, "short")

        bsl::cout << "Testing conversion to int." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_INT, Int, "int")
        TIME_FROM_TO(Short,      "short",       BDEM_INT, Int, "int")
        TIME_FROM_TO(Int,        "int",         BDEM_INT, Int, "int")
        TIME_FROM_TO(Int64,      "int64",       BDEM_INT, Int, "int")
        TIME_FROM_TO(Float,      "float",       BDEM_INT, Int, "int")
        TIME_FROM_TO(Double,     "double",      BDEM_INT, Int, "int")
        TIME_FROM_TO(LDouble,    "long double", BDEM_INT, Int, "int")
        TIME_FROM_TO(String,     "string",      BDEM_INT, Int, "int")

        bsl::cout << "Testing conversion to Int64." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(Short,      "short",       BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(Int,        "int",         BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(Int64,      "int64",       BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(Float,      "float",       BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(Double,     "double",      BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(LDouble,    "long double", BDEM_INT64, Int64, "int64")
        TIME_FROM_TO(String,     "string",      BDEM_INT64, Int64, "int64")

        bsl::cout << "Testing conversion to float." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(Short,      "short",       BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(Int,        "int",         BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(Int64,      "int64",       BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(Float,      "float",       BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(Double,     "double",      BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(LDouble,    "long double", BDEM_FLOAT, Float, "float")
        TIME_FROM_TO(String,     "string",      BDEM_FLOAT, Float, "float")

        bsl::cout << "Testing conversion to double." << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(Short,      "short",       BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(Int,        "int",         BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(Int64,      "int64",       BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(Float,      "float",       BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(Double,     "double",      BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(LDouble,    "long double", BDEM_DOUBLE, Double, "double")
        TIME_FROM_TO(String,     "string",      BDEM_DOUBLE, Double, "double")

        bsl::cout << "Testing conversion to string" << bsl::endl;

        TIME_FROM_TO(Char,       "char",        BDEM_STRING, String, "string")
        TIME_FROM_TO(Short,      "short",       BDEM_STRING, String, "string")
        TIME_FROM_TO(Int,        "int",         BDEM_STRING, String, "string")
        TIME_FROM_TO(Int64,      "int64",       BDEM_STRING, String, "string")
        TIME_FROM_TO(Float,      "float",       BDEM_STRING, String, "string")
        TIME_FROM_TO(Double,     "double",      BDEM_STRING, String, "string")
        TIME_FROM_TO(LDouble,    "long double", BDEM_STRING, String, "string")
        TIME_FROM_TO(String,     "string",      BDEM_STRING, String, "string")
        TIME_FROM_TO(Date,       "date",        BDEM_STRING, String, "string")
        TIME_FROM_TO(Time,       "time",        BDEM_STRING, String, "string")
        TIME_FROM_TO(Datetime,   "datetime",    BDEM_STRING, String, "string")
        TIME_FROM_TO(DateTz,     "dateTz",      BDEM_STRING, String, "string")
        TIME_FROM_TO(TimeTz,     "timeTz",      BDEM_STRING, String, "string")
        TIME_FROM_TO(DatetimeTz, "datetimeTz",  BDEM_STRING, String, "string")

#undef TIME_FROM_TO

        if (verbose) bsl::cout << "Testing fromBdemType.\n"
                               << "---------------------" << bsl::endl;

#define TIME_FROM_TO(FROM_BDEM, FROM_TYPE, FROM_STR, TO_TYPE, TO_STR) {      \
        bsls::Stopwatch timer;                                               \
        int iters = NUM_ITER / 2;                                            \
        int status = bdlmxxx::Convert::fromBdemType(&x##TO_TYPE,                 \
                                                &val##FROM_TYPE,             \
                                                bdlmxxx::ElemType::FROM_BDEM);   \
        int status2 = status;                                                \
        do {                                                                 \
            timer.reset();                                                   \
            iters *= 2;                                                      \
            timer.start();                                                   \
            for (int i = 0; i < iters; ++i) {                                \
                status += bdlmxxx::Convert::fromBdemType(&x##TO_TYPE,            \
                                                &val##FROM_TYPE,             \
                                                bdlmxxx::ElemType::FROM_BDEM);   \
            }                                                                \
            timer.stop();                                                    \
        } while (timer.elapsedTime() < limit);                               \
        ASSERT((0 == status && 0 == status2) || status < status2);           \
        bsl::cout << "\t" << iters << " conversions"                         \
                  << " from " << FROM_STR << " [bdem] to " << TO_STR         \
                  << " in " << timer.elapsedTime() << " seconds ("           \
                  << timer.elapsedTime() / iters << " per conversion)."      \
                  << bsl::endl;                                              \
    }
        // This macro evaluates the 'bdlmxxx::Convert::convert' operation applied
        // from a reference to a 'bdem' 'FROM_BDEM' type to a modifiable 'bdem'
        // reference to an instance of the parameterized 'TO_TYPE', repeatedly
        // so that the total running time exceeds 1s (or the limit passed as
        // second argument after the test case number -1 on the command line),
        // and reports to 'bsl::cout' the statistics of the timing.  The
        // instance of 'FROM_TYPE' is null if the parameter 'NULL_OR_VAL' is
        // "null", otherwise 'NV' should be passed 'val' for some (unspecified)
        // value of 'FROM_TYPE'.

        bsl::cout << "Testing conversion to char." << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,    Char,      "char",        Char, "char")
        TIME_FROM_TO(BDEM_SHORT,   Short,     "short",       Char, "char")
        TIME_FROM_TO(BDEM_INT,     Int,        "int",         Char, "char")
        TIME_FROM_TO(BDEM_INT64,   Int64,      "int64",       Char, "char")
        TIME_FROM_TO(BDEM_FLOAT,   Float,      "float",       Char, "char")
        TIME_FROM_TO(BDEM_DOUBLE,  Double,     "double",      Char, "char")
        TIME_FROM_TO(BDEM_STRING,  String,     "string",      Char, "char")

        bsl::cout << "Testing conversion to short." << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,    Char,       "char",        Short, "short")
        TIME_FROM_TO(BDEM_SHORT,   Short,      "short",       Short, "short")
        TIME_FROM_TO(BDEM_INT,     Int,        "int",         Short, "short")
        TIME_FROM_TO(BDEM_INT64,   Int64,      "int64",       Short, "short")
        TIME_FROM_TO(BDEM_FLOAT,   Float,      "float",       Short, "short")
        TIME_FROM_TO(BDEM_DOUBLE,  Double,     "double",      Short, "short")
        TIME_FROM_TO(BDEM_STRING,  String,     "string",      Short, "short")

        bsl::cout << "Testing conversion to int." << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,    Char,       "char",        Int, "int")
        TIME_FROM_TO(BDEM_SHORT,   Short,      "short",       Int, "int")
        TIME_FROM_TO(BDEM_INT,     Int,        "int",         Int, "int")
        TIME_FROM_TO(BDEM_INT64,   Int64,      "int64",       Int, "int")
        TIME_FROM_TO(BDEM_FLOAT,   Float,      "float",       Int, "int")
        TIME_FROM_TO(BDEM_DOUBLE,  Double,     "double",      Int, "int")
        TIME_FROM_TO(BDEM_STRING,  String,     "string",      Int, "int")

        bsl::cout << "Testing conversion to Int64." << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,    Char,       "char",        Int64, "int64")
        TIME_FROM_TO(BDEM_SHORT,   Short,      "short",       Int64, "int64")
        TIME_FROM_TO(BDEM_INT,     Int,        "int",         Int64, "int64")
        TIME_FROM_TO(BDEM_INT64,   Int64,      "int64",       Int64, "int64")
        TIME_FROM_TO(BDEM_FLOAT,   Float,      "float",       Int64, "int64")
        TIME_FROM_TO(BDEM_DOUBLE,  Double,     "double",      Int64, "int64")
        TIME_FROM_TO(BDEM_STRING,  String,     "string",      Int64, "int64")

        bsl::cout << "Testing conversion to float." << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,    Char,       "char",        Float, "float")
        TIME_FROM_TO(BDEM_SHORT,   Short,      "short",       Float, "float")
        TIME_FROM_TO(BDEM_INT,     Int,        "int",         Float, "float")
        TIME_FROM_TO(BDEM_INT64,   Int64,      "int64",       Float, "float")
        TIME_FROM_TO(BDEM_FLOAT,   Float,      "float",       Float, "float")
        TIME_FROM_TO(BDEM_DOUBLE,  Double,     "double",      Float, "float")
        TIME_FROM_TO(BDEM_STRING,  String,     "string",      Float, "float")

        bsl::cout << "Testing conversion to double." << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,    Char,       "char",        Double, "double")
        TIME_FROM_TO(BDEM_SHORT,   Short,      "short",       Double, "double")
        TIME_FROM_TO(BDEM_INT,     Int,        "int",         Double, "double")
        TIME_FROM_TO(BDEM_INT64,   Int64,      "int64",       Double, "double")
        TIME_FROM_TO(BDEM_FLOAT,   Float,      "float",       Double, "double")
        TIME_FROM_TO(BDEM_DOUBLE,  Double,     "double",      Double, "double")
        TIME_FROM_TO(BDEM_STRING,  String,     "string",      Double, "double")

        bsl::cout << "Testing conversion to string" << bsl::endl;

        TIME_FROM_TO(BDEM_CHAR,       Char,       "char",      String,"string")
        TIME_FROM_TO(BDEM_SHORT,      Short,      "short",     String,"string")
        TIME_FROM_TO(BDEM_INT,        Int,        "int",       String,"string")
        TIME_FROM_TO(BDEM_INT64,      Int64,      "int64",     String,"string")
        TIME_FROM_TO(BDEM_FLOAT,      Float,      "float",     String,"string")
        TIME_FROM_TO(BDEM_DOUBLE,     Double,     "double",    String,"string")
        TIME_FROM_TO(BDEM_STRING,     String,     "string",    String,"string")
        TIME_FROM_TO(BDEM_DATE,       Date,       "date",      String,"string")
        TIME_FROM_TO(BDEM_TIME,       Time,       "time",      String,"string")
        TIME_FROM_TO(BDEM_DATETIME,   Datetime,   "datetime",  String,"string")
        TIME_FROM_TO(BDEM_DATETZ,     DateTz,     "dateTz",    String,"string")
        TIME_FROM_TO(BDEM_TIMETZ,     TimeTz,     "timeTz",    String,"string")
        TIME_FROM_TO(BDEM_DATETIMETZ, DatetimeTz, "datetimeTz",String,"string")

#endif // TEST_IS_ENABLED(-1)

#endif
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
