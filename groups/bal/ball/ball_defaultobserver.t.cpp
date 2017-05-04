// ball_defaultobserver.t.cpp                                         -*-C++-*-
#include <ball_defaultobserver.h>

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_testutil.h>

#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

// Note: on Windows -> WinGDI.h:#define PASSTHROUGH 19
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(PASSTHROUGH)
#undef PASSTHROUGH
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under the test is a simple observer that prints the log
// records to the 'bsl::ostream' supplied at construction.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] DefaultObserver(bsl::ostream *stream)
//
// MANIPULATORS
// [ 2] void publish(const bsl::shared_ptr<const ball::Record>& record,
//                   const ball::Context&)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

}  // close unnamed namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::DefaultObserver Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;      // Supress compiler warning.
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (verbose) cout << "Skipped due to cyclic dependancy." << endl;

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 Log record is correctly formatted.
        //:
        //: 2 Formatted log record is written to the output stream.
        //
        // Plan:
        //: 1 Create the observer object and publish log record.
        //
        // Testing:
        //   DefaultObserver(bsl::ostream * stream);
        //   ~DefaultObserver();
        //   publish(bsl::shared_ptr<Record> &record, const Context&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS"
                          << "\n============================" << endl;

        {
            Obj mX(&bsl::cout);  const Obj& X = mX;
        }

        if (verbose) cout << "Publish a single message." << endl;
        {
            bsl::ostringstream os;
            Obj mX(&os);

            ball::RecordAttributes fixed;

            bdlt::Datetime timestamp(2017, 4, 1);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(2);
            fixed.setThreadID(3);
            fixed.setSeverity(ball::Severity::e_TRACE);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(191);
            fixed.setMessage("Log Message");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();
            record->setFixedFields(fixed);

            mX.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                             0,
                                             1));

            ASSERTV(os.str(),
                    "\n01APR2017_00:00:00.000 2 3 TRACE test.cpp 191"
                    "  Log Message \n"
                    == os.str());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        if (verbose)
            cout << "Publish a single message (deprecated c'tor)." << endl;
        {
            Obj X(bsl::cout);

            ball::RecordAttributes fixed;
            ball::UserFields  userValues;

            bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
            fixed.setTimestamp(now);
            fixed.setProcessID(100);
            fixed.setThreadID(0);
            X.publish(ball::Record(fixed, userValues),
                    ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));
        }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << "Publish a single message." << endl;
        {
            Obj X(&bsl::cout);

            ball::RecordAttributes fixed;
            ball::UserFields       userValues;

            bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
            fixed.setTimestamp(now);
            fixed.setProcessID(100);
            fixed.setThreadID(0);
            X.publish(ball::Record(fixed, userValues),
                      ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));
        }

        if (verbose) cout << "Publish a sequence of three messages." << endl;
        {
            Obj X(&bsl::cout);

            ball::RecordAttributes fixed;
            ball::UserFields  userValues;

            const int NUM_MESSAGES = 3;
            for (int n = 0; n < NUM_MESSAGES; ++n) {
                bdlt::Datetime now =
                                    bdlt::EpochUtil::convertFromTimeT(time(0));
                fixed.setTimestamp(now);
                fixed.setProcessID(201 + n);
                fixed.setThreadID(31 + n);
                X.publish(ball::Record(fixed, userValues),
                          ball::Context(ball::Transmission::e_TRIGGER,
                                        n,
                                        NUM_MESSAGES));
            }
        }
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
