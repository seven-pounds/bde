// ball_defaultobserver.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_DEFAULTOBSERVER
#define INCLUDED_BALL_DEFAULTOBSERVER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a default observer that emits log records to 'stdout'.
//
//@CLASSES:
//    ball::DefaultObserver: observer that outputs log records to 'stdout'
//
//@SEE_ALSO: ball_record, ball_context, ball_loggermanager
//
//@AUTHOR: Banyar Aung
//
//@DESCRIPTION: This component provides a concrete implementation of the
// 'ball::Observer' protocol for receiving and processing log records:
//..
//                ,---------------------.
//               ( ball::DefaultObserver )
//                 `--------------------'
//                           |              ctor
//                           |              publish
//                           V
//                    ,--------------.
//                   ( ball::Observer )
//                    `--------------'
//                                          dtor
//..
// 'ball::DefaultObserver' is a concrete class derived from 'ball::Observer'
// that processes the log records it receives through its 'publish' method by
// printing them to 'stdout'.  Given its minimal functionality,
// 'ball::DefaultObserver' is intended for development use only.
// 'ball::DefaultObserver' is not recommended for use in a production
// environment.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Publication Through Logger Manager
///- - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using a 'ball::DefaultObserver' within a 'ball'
// logging system.
//
// First, we initialize 'ball' logging subsystem with the default
// configuration:
//..
//  ball::LoggerManagerConfiguration configuration;
//  ball::LoggerManagerScopedGuard   guard(configuration);
//
//  ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Then, we create a shared pointer to a 'ball::StreamObserver' object,
// 'observerPtr', passing a pointer to the 'bsl::cout'.
//..
//  bslma::Allocator *alloc =  bslma::Default::globalAllocator(0);
//  bsl::shared_ptr<ball::Observer> observerPtr(
//                               new(*alloc) ball::DefaultObserver(&bsl::cout),
//                               alloc);
//..
// Then, we register the default observer with the logger manager.  Upon
// successful registration, the observer will start to receive log records via
// 'publish' method:
//..
//  int rc = manager.registerObserver(observerPtr, "observer");
//  assert(0 == rc);
//..
// Next, we set the log category and log few messages with different logging
// severity:
//..
//  BALL_LOG_SET_CATEGORY("ball::DefaultObserver");
//
//  BALL_LOG_INFO << "Info Log Message."    << BALL_LOG_END;
//  BALL_LOG_WARN << "Warning Log Message." << BALL_LOG_END;
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_OBSERVER
#include <ball_observer.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace ball {

class Context;
class Record;

                           // =====================
                           // class DefaultObserver
                           // =====================

class DefaultObserver : public Observer {
    // This class provides a concrete implementation of the 'Observer'
    // protocol.  The 'publish' method of this class outputs the log records
    // that it receives to an instance of 'bsl::ostream' supplied at
    // construction.

    // DATA
    bsl::ostream *d_stream_p;  // output sink for log records
    bslmt::Mutex  d_mutex;     // serializes concurrent calls to 'publish'

    // NOT IMPLEMENTED
    DefaultObserver(const DefaultObserver&);
    DefaultObserver& operator=(const DefaultObserver&);

  public:
    using Observer::publish;

    // CREATORS
    explicit DefaultObserver(bsl::ostream *stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    explicit DefaultObserver(bsl::ostream& stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.
        //
        // DEPRECATED: replaced by 'DefaultObserver(bsl::ostream *stream)'
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    virtual ~DefaultObserver();
        // Destroy this default observer.

    // MANIPULATORS
    virtual void publish(const Record& record, const Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.

    virtual void publish(const bsl::shared_ptr<const Record>& record,
                         const Context&                       context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.  Print 'record' and 'context' to the 'bsl::ostream'
        // supplied at construction.  The behavior is undefined if 'record' or
        // 'context' is modified during the execution of this method.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class DefaultObserver
                           // ---------------------

// CREATORS
inline
DefaultObserver::DefaultObserver(bsl::ostream *stream)
: d_stream_p(stream)
{
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
DefaultObserver::DefaultObserver(bsl::ostream& stream)
: d_stream_p(&stream)
{
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

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
