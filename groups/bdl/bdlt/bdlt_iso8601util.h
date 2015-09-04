// bdlt_iso8601util.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLT_ISO8601UTIL
#define INCLUDED_BDLT_ISO8601UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide conversions between date/time objects and ISO 8601 strings.
//
//@CLASSES:
//  bdlt::Iso8601Util: namespace for ISO 8601 date/time conversion functions
//
//@SEE_ALSO: bdlt_iso8601utilconfiguration
//
//@DESCRIPTION: This component provides a namespace, 'bdlt::Iso8601Util',
// containing functions that convert 'bdlt' date, time, and datetime objects to
// and from ("generate" and "parse", respectively) corresponding string
// representations that are compliant with the ISO 8601 standard.  The version
// of the ISO 8601 standard that is the basis for this component can be found
// at:
//..
//  http://dotat.at/tmp/ISO_8601-2004_E.pdf
//..
// In general terms, 'Iso8601Util' functions support what ISO 8601 refers to as
// *complete* *representations* in *extended* *format*.  We first present a
// brief overview before delving into the details of the ISO 8601
// representations that are supported for each of the relevant 'bdlt'
// vocabulary types.
//
// Each function that *generates* ISO 8601 strings (named 'generate' and
// 'generateRaw') takes a 'bdlt' object and a 'char *' buffer or
// 'bsl::ostream', and writes an ISO 8601 representation of the object to the
// buffer or stream.  The "raw" functions are distinguished from their
// non-"raw" counterparts in two respects:
//
//: o The length of the 'char *' buffer is not supplied to the "raw" functions.
//:
//: o The "raw" functions do not output a null terminator.
//
// Since the generate functions always succeed, no status value is returned.
// Instead, either the number of characters output to the 'char *' buffer or a
// reference to the stream is returned.  (Note that the generating functions
// also take an optional 'bdlt::Iso8601UtilConfiguration' object, which is
// discussed shortly.)
//
// Each function that *parses* ISO 8601 strings (named 'parse') take the
// address of a target 'bdlt' object and a 'const char *', and loads the object
// with the result of parsing the character string.  Since parsing can fail,
// the parse functions return an 'int' status value (0 for success and a
// non-zero value for failure).  Note that, besides elementary syntactical
// considerations, the validity of parsed strings are subject to the semantic
// constraints imposed by the various 'isValid*' class methods, (i.e.,
// 'Date::isValidYearMonthDay', 'Time::isValid', etc.).
//
///ISO 8601 String Generation
///--------------------------
// Strings produced by the 'generate' and 'generateRaw' functions are a
// straightforward transposition of the attributes of the source 'bdlt' value
// into an appropriate ISO 8601 format, and are best illustrated by a few
// examples.  Note that for any type having a time component ('Time', 'TimeTz',
// 'Datetime', and 'DatetimeTz'), the fractional second is always generated,
// and always with three decimal digits:
//..
//  +--------------------------------------+---------------------------------+
//  |             Object Value             |    Generated ISO 8601 String    |
//  +======================================+=================================+
//  |  Date(2002, 03, 17)                  |  2002-03-17                     |
//  +--------------------------------------+---------------------------------+
//  |  Time(15, 46, 09, 330)               |  15:46:09.330                   |
//  +--------------------------------------+---------------------------------+
//  |  Datetime(Date(2002, 03, 17)         |                                 |
//  |           Time(15, 46, 09, 330))     |  2002-03-17T15:46:09.330        |
//  +--------------------------------------+---------------------------------+
//  |  DateTz(Date(2002, 03, 17), -120)    |  2002-03-17-02:00               |
//  +--------------------------------------+---------------------------------+
//  |  TimeTz(Time(15, 46, 09, 330), 270)  |  15:46:09.330+04:30             |
//  +--------------------------------------+---------------------------------+
//  |  DatetimeTz(Datetime(                |                                 |
//  |              Date(2002, 03, 17),     |                                 |
//  |              Time(15, 46, 09, 330)), |                                 |
//  |             0)                       |  2002-03-17T15:46:09.330+00:00  |
//  +--------------------------------------+---------------------------------+
//..
///Configuration
///- - - - - - -
// The 'generate' and 'generateRaw' functions come in matching pairs, where the
// two functions in each pair are distinguished by whether or not an
// 'Iso8601UtilConfiguration' object is supplied.  This optional argument
// enables configuration of three aspects of ISO 8601 string generation:
//: o The decimal sign to use in fractional seconds: '.' or ','.
//:
//: o Whether ':' is optional in zone designators.
//:
//: o Whether 'Z' is output for the zone designator instead of '+00:00' (UTC).
//
// 'Iso8601UtilConfiguration' has three attributes that directly correspond to
// these aspects.  In addition, for generate methods that are not supplied with
// a configuration argument, a process-wide configuration takes effect.  See
// 'bdlt_iso8601utilconfiguration' for details.
//
///ISO 8601 String Parsing
///-----------------------
// The parse functions accept *all* strings that are produced by the generate
// functions.  In addition, the parse functions accept some variation in the
// generated strings, the details of which are discussed next.  Note that the
// parse methods are not configurable like the generate methods (i.e., via an
// optional 'Iso8601UtilConfiguration' argument).  Moreover, the process-wide
// configuration has no effect on parsing either.  Instead, the parse methods
// automatically accept '.' or ',' as the decimal sign in fractional seconds,
// and treat '+00:00', '+0000', and 'Z' as equivalent zone designators (all
// denoting UTC).
//
///Zone Designators
/// - - - - - - - -
// The zone designator is optional, and can be present when parsing for *any*
// type, i.e., even for 'Date', 'Time', and 'Datetime'.  If a zone designator
// is parsed for a 'Date', it must be valid, so it can affect the status value
// that is returned in that case, but it is otherwise ignored.  For 'Time' and
// 'Datetime', any zone designator present in the parsed string will affect the
// resulting object value (unless the zone designator denotes UTC) because the
// result is converted to GMT.  If the zone designator is absent, it is treated
// as if '+00:00' were specified:
//..
//  +------------------------------------+-----------------------------------+
//  |       Parsed ISO 8601 String       |        Result Object Value        |
//  +====================================+===================================+
//  |  2002-03-17-02:00                  |  Date(2002, 03, 17)               |
//  |                                    |  # zone designator ignored        |
//  +------------------------------------+-----------------------------------+
//  |  2002-03-17-02:65                  |  Date: parsing fails              |
//  |                                    |  # invalid zone designator        |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330+04:30                |  Time(11, 16, 09, 330)            |
//  |                                    |  # converted to GMT               |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330+04:30                |  TimeTz(Time(15, 46, 09, 330),    |
//  |                                    |         270)                      |
//  +------------------------------------+-----------------------------------+
//  |  15:46:09.330                      |  TimeTz(Time(15, 46, 09, 330),    |
//  |                                    |         0)                        |
//  |                                    |  # implied '+00:00'               |
//  +------------------------------------+-----------------------------------+
//  |  2002-03-17T23:46:09.222-5:00      |  Datetime(Date(2002, 03, 18),     |
//  |                                    |           Time(04, 46, 09, 222))  |
//  |                                    |  # carry into 'day' attribute     |
//  |                                    |  # when converted to GMT          |
//  +------------------------------------+-----------------------------------+
//..
// In the last example above, the conversion to GMT incurs a carry into the
// 'day' attribute of the 'Date' component of the resulting 'Datetime' value.
// Note that if such a carry would cause an underflow or overflow at the
// extreme ends of the valid range of dates (0001/01/01 and 9999/12/31), then
// parsing for 'Datetime' would fail.
//
///Fractional Seconds
/// - - - - - - - - -
// The fractional second is optional.  When the fractional second is absent, it
// is treated as if '.0' were specified.  When the fractional second is
// present, it can have one or more digits (i.e., it can contain more than
// three).  If more than three digits are included in the fractional second,
// values greater than or equal to .9995 are rounded up to 1000 milliseconds.
// This incurs a carry of one second into the 'second' attribute of the 'Time'
// component:
//..
//  +--------------------------------------+---------------------------------+
//  |        Parsed ISO 8601 String        |      Result Object Value        |
//  +======================================+=================================+
//  |  15:46:09.1                          |  Time(15, 46, 09, 100)          |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09-5:00                       |  TimeTz(Time(15, 46, 09, 000),  |
//  |                                      |         -300)                   |
//  |                                      |  # implied '.0'                 |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09.99949                      |  Time(15, 46, 09, 999)          |
//  |                                      |  # truncate last two digits     |
//  +--------------------------------------+---------------------------------+
//  |  15:46:09.9995                       |  Time(15, 46, 10, 000)          |
//  |                                      |  # round up and carry           |
//  +--------------------------------------+---------------------------------+
//..
// Note that if a carry due to rounding of the fractional second would cause an
// overflow at the extreme upper end of the valid range of dates (i.e.,
// 9999/12/31), then parsing for 'Datetime' and 'DatetimeTz' would fail.
//
///Leap Seconds
/// - - - - - -
// Although leap seconds are not representable by 'bdlt::Time', positive leap
// seconds are supported by the parse functions.  A leap second is recognized
// when the value parsed for the 'second' attribute of a 'Time' is 60.  When a
// leap second is detected, the 'second' attribute is taken to be 59, so that
// the value of the 'Time' object can be validly set, then an additional second
// is added to the object.  Note that the possible carry incurred by a leap
// second has the same potential for overflow as may occur with fractional
// seconds that are rounded up (although in admittedly pathological cases).
//
// TBD Should we tighten up our handling of leap seconds?  The ISO 8601
// specification seems to suggest that a positive leap second can only be
// represented as "23:59:60Z", but we allow 60 (for the 'second') to be coupled
// with any 'hour', 'minute', 'millisecond', and zone designator.
//
///The Time 24:00
/// - - - - - - -
// According to the ISO 8601 specification, the time 24:00 is interpreted as
// midnight, i.e., the last instant of a day.  However, this concept is not
// representable by 'bdlt'.  Furthermore, 24:00 can only be paired (i.e., in a
// 'Datetime' or 'DatetimeTz' object) with the default date, 0001/01/01, and
// the timezone offset (if any) must be 0.  If 24:00 is detected while parsing
// an ISO 8601 string, the 'hour' attribute of 'Time' is set to 24 only if
// needed to preserve the default value for a 'Time', 'TimeTz', 'Datetime', or
// 'DatetimeTz'.  Otherwise, if 24:00 is paired with any date other than the
// default, the resulting object is set to represent the first instant of the
// next day (i.e., the date component is advanced by one day and the time
// component is set to 00:00).  Note that parsing fails if the zone designator
// is not equivalent to "+00:00", whether implied or explicit, when 24:00 is
// encountered:
//..
//  +------------------------------------+-----------------------------------+
//  |       Parsed ISO 8601 String       |        Result Object Value        |
//  +====================================+===================================+
//  |  24:00:00.000                      |  Time(24, 0, 0, 0)                |
//  |                                    |  # preserve default 'Time' value  |
//  +------------------------------------+-----------------------------------+
//  |  24:00:00.000-4:00                 |  TimeTz: parsing fails            |
//  |                                    |  # zone designator not UTC        |
//  +------------------------------------+-----------------------------------+
//  |  0001-01-01T24:00:00.000           |  Datetime(Date(0001, 01, 01),     |
//  |                                    |           Time(24, 0, 0, 0))      |
//  |                                    |  # preserve 'Datetime' default    |
//  |                                    |  # value                          |
//  +------------------------------------+-----------------------------------+
//  |  2002-03-17T24:00:00.000           |  Datetime(Date(2002, 03, 18),     |
//  |                                    |           Time(0, 0, 0, 0))       |
//  |                                    |  # first instant of the next day  |
//  |                                    |  # if 'Date' not 0001/01/01       |
//  +------------------------------------+-----------------------------------+
//..
// TBD the implementation has not yet been changed to match the above table
// with respect to 24:00 being treated as the first instant of the next day.
// This warrants some discussion before doing so.
//
///Summary of Supported ISO 8601 Representations
///- - - - - - - - - - - - - - - - - - - - - - -
// The syntax description below summarizes the ISO 8601 string representations
// supported by this component.  Although not quoted (for readability),
// '[+-:.,TZ]' are literal characters that can occur in ISO 8601 strings.  The
// characters '[YMDhms]' each denote a decimal digit, '{}' brackets optional
// elements, '()' is used for grouping, and '|' separates alternatives:
//..
// <Generated Date>        ::=  <DATE>
//
// <Parsed Date>           ::=  <Parsed DateTz>
//
// <Generated DateTz>      ::=  <DATE><ZONE>
//
// <Parsed DateTz>         ::=  <DATE>{<ZONE>}
//
// <Generated Time>        ::=  <TIME FIXED>
//
// <Parsed Time>           ::=  <Parsed TimeTz>
//
// <Generated TimeTz>      ::=  <TIME FIXED><ZONE>
//
// <Parsed TimeTz>         ::=  <TIME FLEXIBLE>{<ZONE>}
//
// <Generated Datetime>    ::=  <DATE>T<TIME FIXED>
//
// <Parsed Datetime>       ::=  <Parsed DatetimeTz>
//
// <Generated DatetimeTz>  ::=  <DATE>T<TIME FIXED><ZONE>
//
// <Parsed DatetimeTz>     ::=  <DATE>T<TIME FLEXIBLE>{<ZONE>}
//
// <DATE>                  ::=  YYYY-MM-DD
//
// <TIME FIXED>            ::=  hh:mm:ss(.|,)sss   # exactly three digits in
//                                                 # the fractional second
//
// <TIME FLEXIBLE>         ::=  hh:mm:ss{(.|,)s+}  # one or more digits in the
//                                                 # fractional second
//
// <ZONE>                  ::=  (+|-)hh{:}mm|Z     # zone designator
//..
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::Iso8601Util' Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of one 'generate' function and two
// 'parse' functions.
//
// First, we construct a few objects that are prerequisites for this and the
// following example:
//..
//  const bdlt::Date date(2005, 1, 31);     // 2005/01/31
//  const bdlt::Time time(8, 59, 59, 123);  // 08::59::59.123
//  const int        tzOffset = 240;        // +04:00 (four hours west of GMT)
//..
// Then, we construct a 'bdlt::DatetimeTz' object for which a corresponding ISO
// 8601-compliant string will be generated shortly:
//..
//  const bdlt::DatetimeTz sourceDatetimeTz(bdlt::Datetime(date, time),
//                                          tzOffset);
//..
// For comparison with the ISO 8601 string generated below, note that streaming
// the value of 'sourceDatetimeTz' to 'stdout':
//..
//  bsl::cout << sourceDatetimeTz << bsl::endl;
//..
// produces:
//..
//  31JAN2005_08:59:59.123+0400
//..
// Next, we use a 'generate' function to produce an ISO 8601-compliant string
// for 'sourceDatetimeTz', writing the output to a 'bsl::ostringstream', and
// assert that both the return value and the string that is produced are as
// expected:
//..
//  bsl::ostringstream  oss;
//  const bsl::ostream& ret =
//                         bdlt::Iso8601Util::generate(oss, sourceDatetimeTz);
//  assert(&oss == &ret);
//
//  const bsl::string iso8601 = oss.str();
//  assert(iso8601 == "2005-01-31T08:59:59.123+04:00");
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Now, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::DatetimeTz' object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceDatetimeTz'):
//..
//  bdlt::DatetimeTz targetDatetimeTz;
//
//  int rc = bdlt::Iso8601Util::parse(&targetDatetimeTz,
//                                    iso8601.c_str(),
//                                    static_cast<int>(iso8601.length()));
//  assert(               0 == rc);
//  assert(sourceDatetimeTz == targetDatetimeTz);
//..
// Finally, we parse the 'iso8601' string a second time, this time loading the
// result into a 'bdlt::Datetime' object (instead of a 'bdlt::DatetimeTz'):
//..
//  bdlt::Datetime targetDatetime;
//
//  rc = bdlt::Iso8601Util::parse(&targetDatetime,
//                                iso8601.c_str(),
//                                static_cast<int>(iso8601.length()));
//  assert(                             0 == rc);
//  assert(sourceDatetimeTz.utcDatetime() == targetDatetime);
//..
// Note that this time the value of the target object has been converted to
// GMT.
//
///Example 2: Configuring ISO 8601 String Generation
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates use of a 'bdlt::Iso8601UtilConfiguration' object
// to influence the format of the ISO 8601 strings that are generated by this
// component by passing that configuration object to 'generate'.  We also take
// this opportunity to illustrate the flavor of the 'generate' functions that
// outputs to a 'char *' buffer of a specified length.
//
// First, we construct a 'bdlt::TimeTz' object for which a corresponding ISO
// 8601-compliant string will be generated shortly:
//..
//  const bdlt::TimeTz sourceTimeTz(time, tzOffset);
//..
// For comparison with the ISO 8601 string generated below, note that streaming
// the value of 'sourceTimeTz' to 'stdout':
//..
//  bsl::cout << sourceTimeTz << bsl::endl;
//..
// produces:
//..
//  08:59:59.123+0400
//..
// Then, we construct the 'bdlt::Iso8601UtilConfiguration' object that
// indicates how we would like to affect the generated output ISO 8601 string.
// In this case, we want to use ',' as the decimal sign (in fractional seconds)
// and omit the ':' in zone designators:
//..
//  bdlt::Iso8601UtilConfiguration configuration;
//  configuration.setOmitColonInZoneDesignator(true);
//  configuration.setUseCommaForDecimalSign(true);
//..
// Next, we define the 'char *' buffer that will be used to stored the
// generated string.  A buffer of size 'bdlt::Iso8601Util::k_TIMETZ_STRLEN + 1'
// is large enough to hold any string generated by this component for a
// 'bdlt::TimeTz' object, including a null terminator:
//..
//  const int BUFLEN = bdlt::Iso8601Util::k_TIMETZ_STRLEN + 1;
//  char      buffer[BUFLEN];
//..
// Then, we use a 'generate' function that accepts our 'configuration' to
// produce an ISO 8601-compliant string for 'sourceTimeTz', this time writing
// the output to a 'char *' buffer, and assert that both the return value and
// the string that is produced are as expected.  Note that in comparing the
// return value against 'BUFLEN - 2' we account for the omission of the ':'
// from the zone designator, and also for the fact that, although a null
// terminator was generated, it is not included in the character count returned
// by 'generate'.  Also note that we use 'bsl::strcmp' to compare the resulting
// string knowing that we supplied a buffer having sufficient capacity to
// accommodate a null terminator:
//..
//  rc = bdlt::Iso8601Util::generate(buffer,
//                                   sourceTimeTz,
//                                   BUFLEN,
//                                   configuration);
//  assert(BUFLEN - 2 == rc);
//  assert(         0 == bsl::strcmp(buffer, "08:59:59,123+0400"));
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Next, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::TimeTz' object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceTimeTz').  Note that 'BUFLEN - 2' is passed and *not*
// 'BUFLEN' because the former indicates the correct number of characters in
// 'buffer' that we wish to parse:
//..
//  bdlt::TimeTz targetTimeTz;
//
//  rc = bdlt::Iso8601Util::parse(&targetTimeTz, buffer, BUFLEN - 2);
//
//  assert(           0 == rc);
//  assert(sourceTimeTz == targetTimeTz);
//..
// Finally, we parse the string in 'buffer' a second time, this time loading
// the result into a 'bdlt::Time' object (instead of a 'bdlt::TimeTz'):
//..
//  bdlt::Time targetTime;
//
//  rc = bdlt::Iso8601Util::parse(&targetTime, buffer, BUFLEN - 2);
//  assert(                     0 == rc);
//  assert(sourceTimeTz.utcTime() == targetTime);
//..
// Note that this time the value of the target object has been converted to
// GMT.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_ISO8601UTILCONFIGURATION
#include <bdlt_iso8601utilconfiguration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {
namespace bdlt {

class Date;
class DateTz;
class Datetime;
class DatetimeTz;
class Time;
class TimeTz;

class Iso8601UtilConfiguration;

                            // ==================
                            // struct Iso8601Util
                            // ==================

struct Iso8601Util {
    // This 'struct' provides a namespace for a suite of pure functions that
    // perform conversions between objects of 'bdlt' vocabulary type and their
    // ISO 8601 representations.  Each 'generate' and 'generateRaw' method
    // takes a 'bdlt' object (of type 'Date', 'DateTz', 'Time', 'TimeTz',
    // 'Datetime', or 'DatetimeTz') and outputs its corresponding ISO 8601
    // representation to a user-supplied character buffer or 'bsl::ostream'.
    // The 'parse' methods effect the opposite conversion in that they populate
    // a 'bdlt' object from the result of parsing an ISO 8601 representation.

    // TYPES
    enum {
        // This enumeration defines fixed lengths for the ISO 8601
        // representations of date, time, and datetime values.

        k_DATE_STRLEN       = 10,  // 'bdlt::Date'
        k_DATETZ_STRLEN     = 16,  // 'bdlt::DateTz'

        k_TIME_STRLEN       = 12,  // 'bdlt::Time'
        k_TIMETZ_STRLEN     = 18,  // 'bdlt::TimeTz'

        k_DATETIME_STRLEN   = 23,  // 'bdlt::Datetime'
        k_DATETIMETZ_STRLEN = 29,  // 'bdlt::DatetimeTz'

        k_MAX_STRLEN        = k_DATETIMETZ_STRLEN
    };

    // CLASS METHODS
    static int generate(char                            *buffer,
                        const Date&                      object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const Date&                      object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const Time&                      object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const Time&                      object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const Datetime&                  object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const Datetime&                  object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const DateTz&                    object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const DateTz&                    object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const TimeTz&                    object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const TimeTz&                    object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
    static int generate(char                            *buffer,
                        const DatetimeTz&                object,
                        int                              bufferLength);
    static int generate(char                            *buffer,
                        const DatetimeTz&                object,
                        int                              bufferLength,
                        const Iso8601UtilConfiguration&  configuration);
        // Write the ISO 8601 representation of the specified 'object' to the
        // specified 'buffer', truncating (if necessary) to the specified
        // 'bufferLength'.  Optionally specify a 'configuration' to affect the
        // format of the generated string.  If 'configuration' is not supplied,
        // the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.  Return
        // the number of characters in the formatted string before truncation
        // (not counting a null terminator).  If 'bufferLength' indicates
        // sufficient capacity, 'buffer' is null terminated.  The behavior is
        // undefined unless '0 <= bufferLength'.  Note that a buffer of size
        // 'k_MAX_STRLEN + 1' is large enough to hold any string generated by
        // this component (counting a null terminator, if any).

    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Date&                     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Date&                     object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Time&                     object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Time&                     object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Datetime&                 object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const Datetime&                 object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateTz&                   object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DateTz&                   object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeTz&                   object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const TimeTz&                   object,
                                const Iso8601UtilConfiguration& configuration);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeTz&               object);
    static bsl::ostream& generate(
                                bsl::ostream&                   stream,
                                const DatetimeTz&               object,
                                const Iso8601UtilConfiguration& configuration);
        // Write the ISO 8601 representation of the specified 'object' to the
        // specified 'stream'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.  Return
        // a reference to 'stream'.  Note that 'stream' is not null terminated.

    static int generateRaw(char                            *buffer,
                           const Date&                      object);
    static int generateRaw(char                            *buffer,
                           const Date&                      object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const Time&                      object);
    static int generateRaw(char                            *buffer,
                           const Time&                      object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const Datetime&                  object);
    static int generateRaw(char                            *buffer,
                           const Datetime&                  object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const DateTz&                    object);
    static int generateRaw(char                            *buffer,
                           const DateTz&                    object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const TimeTz&                    object);
    static int generateRaw(char                            *buffer,
                           const TimeTz&                    object,
                           const Iso8601UtilConfiguration&  configuration);
    static int generateRaw(char                            *buffer,
                           const DatetimeTz&                object);
    static int generateRaw(char                            *buffer,
                           const DatetimeTz&                object,
                           const Iso8601UtilConfiguration&  configuration);
        // Write the ISO 8601 representation of the specified 'object' to the
        // specified 'buffer'.  Optionally specify a 'configuration' to affect
        // the format of the generated string.  If 'configuration' is not
        // supplied, the process-wide default value
        // 'Iso8601UtilConfiguration::defaultConfiguration()' is used.  Return
        // the number of characters in the formatted string.  'buffer' is not
        // null terminated.  The behavior is undefined unless 'buffer' has
        // sufficient capacity.  Note that a buffer of size 'k_MAX_STRLEN + 1'
        // is large enough to hold any string generated by this component
        // (counting a null terminator, if any).

    static int parse(Date *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified ISO
        // 8601 'string' as a 'Date' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYY-MM-DD{(+|-)hh{:}mm|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If the optional zone designator
        // is present in 'string', it is parsed but ignored.  The behavior is
        // undefined unless '0 <= length'.

    static int parse(Time *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified ISO
        // 8601 'string' as a 'Time' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than three digits is present in 'string', it is rounded
        // to the nearest value in milliseconds.  If the optional zone
        // designator is present in 'string', the resulting 'Time' value is
        // converted to the equivalent GMT time; if the zone designator is
        // absent, GMT is assumed.  TBD doc the treatment of leap seconds here
        // after it is decided how they will be handled.  If the "hh:mm:ss"
        // portion of 'string' is "24:00:00", then the fractional second must
        // be absent or 0, and the zone designator must be absent or indicate
        // GMT.  The behavior is undefined unless '0 <= length'.

    static int parse(Datetime *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified ISO
        // 8601 'string' as a 'Datetime' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than three digits is present in 'string', it is rounded
        // to the nearest value in milliseconds.  If the optional zone
        // designator is present in 'string', the resulting 'Datetime' value is
        // converted to the equivalent GMT value; if the zone designator is
        // absent, GMT is assumed.  TBD doc the treatment of leap seconds here
        // after it is decided how they will be handled.  If the "hh:mm:ss"
        // portion of 'string' is "24:00:00", then the fractional second must
        // be absent or 0, and the zone designator must be absent or indicate
        // GMT.  The behavior is undefined unless '0 <= length'.

    static int parse(DateTz *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified ISO
        // 8601 'string' as a 'DateTz' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYY-MM-DD{(+|-)hh{:}mm|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If the optional zone designator
        // is not present in 'string', GMT is assumed.  The behavior is
        // undefined unless '0 <= length'.

    static int parse(TimeTz *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified ISO
        // 8601 'string' as a 'TimeTz' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  hh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than three digits is present in 'string', it is rounded
        // to the nearest value in milliseconds.  If the optional zone
        // designator is not present in 'string', GMT is assumed.  TBD doc the
        // treatment of leap seconds here after it is decided how they will be
        // handled.  If the "hh:mm:ss" portion of 'string' is "24:00:00", then
        // the fractional second must be absent or 0, and the zone designator
        // must be absent or indicate GMT.  The behavior is undefined unless
        // '0 <= length'.

    static int parse(DatetimeTz *result, const char *string, int length);
        // Parse the specified initial 'length' characters of the specified ISO
        // 8601 'string' as a 'DatetimeTz' value, and load the value into the
        // specified 'result'.  Return 0 on success, and a non-zero value (with
        // no effect) otherwise.  'string' is assumed to be of the form:
        //..
        //  YYYY-MM-DDThh:mm:ss{(.|,)s+}{(+|-)hh{:}mm|Z}
        //..
        // *Exactly* 'length' characters are parsed; parsing will fail if a
        // proper prefix of 'string' matches the expected format, but the
        // entire 'length' characters do not.  If an optional fractional second
        // having more than three digits is present in 'string', it is rounded
        // to the nearest value in milliseconds.  If the optional zone
        // designator is not present in 'string', GMT is assumed.  TBD doc the
        // treatment of leap seconds here after it is decided how they will be
        // handled.  If the "hh:mm:ss" portion of 'string' is "24:00:00", then
        // the fractional second must be absent or 0, and the zone designator
        // must be absent or indicate GMT.  The behavior is undefined unless
        // '0 <= length'.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                            // ------------------
                            // struct Iso8601Util
                            // ------------------

// CLASS METHODS
inline
int Iso8601Util::generate(char *buffer, const Date& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generate(char *buffer, const Time& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int
Iso8601Util::generate(char *buffer, const Datetime& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generate(char *buffer, const DateTz& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generate(char *buffer, const TimeTz& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int
Iso8601Util::generate(char *buffer, const DatetimeTz& object, int bufferLength)
{
    BSLS_ASSERT_SAFE(buffer);
    BSLS_ASSERT_SAFE(0 <= bufferLength);

    return generate(buffer,
                    object,
                    bufferLength,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const Date& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Date&                     object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATE_STRLEN + 1];

    const int len = generate(buffer, object, k_DATE_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_DATE_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const Time& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Time&                     object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_TIME_STRLEN + 1];

    const int len = generate(buffer, object, k_TIME_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_TIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const Datetime& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const Datetime&                 object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATETIME_STRLEN + 1];

    const int len = generate(buffer, object, k_DATETIME_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_DATETIME_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const DateTz& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DateTz&                   object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATETZ_STRLEN + 1];

    const int len = generate(buffer, object, k_DATETZ_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_DATETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream& Iso8601Util::generate(bsl::ostream& stream, const TimeTz& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const TimeTz&                   object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_TIMETZ_STRLEN + 1];

    const int len = generate(buffer, object, k_TIMETZ_STRLEN, configuration);
    BSLS_ASSERT_SAFE(k_TIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
bsl::ostream&
Iso8601Util::generate(bsl::ostream& stream, const DatetimeTz& object)
{
    return generate(stream,
                    object,
                    Iso8601UtilConfiguration::defaultConfiguration());
}

inline
bsl::ostream& Iso8601Util::generate(
                                 bsl::ostream&                   stream,
                                 const DatetimeTz&               object,
                                 const Iso8601UtilConfiguration& configuration)
{
    char buffer[k_DATETIMETZ_STRLEN + 1];

    const int len = generate(buffer,
                             object,
                             k_DATETIMETZ_STRLEN,
                             configuration);
    BSLS_ASSERT_SAFE(k_DATETIMETZ_STRLEN >= len);

    return stream.write(buffer, len);
}

inline
int Iso8601Util::generateRaw(char *buffer, const Date& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Time& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const Datetime& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DateTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const TimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

inline
int Iso8601Util::generateRaw(char *buffer, const DatetimeTz& object)
{
    BSLS_ASSERT_SAFE(buffer);

    return generateRaw(buffer,
                       object,
                       Iso8601UtilConfiguration::defaultConfiguration());
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
