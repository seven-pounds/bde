// ball_loggermanager.cpp                                             -*-C++-*-
#include <ball_loggermanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggermanager_cpp,"$Id$ $CSID$")

#include <ball_attributecontext.h>
#include <ball_context.h>
#include <ball_fixedsizerecordbuffer.h>
#include <ball_loggermanagerdefaults.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_streamobserver.h>           // for testing only
#include <ball_testobserver.h>             // for testing only

#include <bdlf_bind.h>
#include <bdlf_memfn.h>

#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>

#include <bslma_default.h>
#include <bslma_managedptr.h>

#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bslmt_qlock.h>
#include <bslmt_readlockguard.h>
#include <bslmt_threadutil.h>
#include <bslmt_writelockguard.h>

#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_new.h>            // placement 'new' syntax
#include <bsl_sstream.h>
#include <bsl_string.h>

// ============================================================================
//                           IMPLEMENTATION NOTES
// ----------------------------------------------------------------------------
//
///Single-Threaded vs. Multi-Threaded Logger Manager
///-------------------------------------------------
// Prior releases of 'ball' were appropriate only for single-threaded
// applications.  Thus, 'ball::LoggerManager::singleton().getLogger()' returns
// a reference to an instance of 'ball::Logger' useful for logging.  The same
// 'ball::Logger' is returned each time 'getLogger()' is called.  In this
// multi-threaded release, the overloaded 'allocateLogger' method returns a
// distinct instance of 'ball::Logger' each time that it is called.  This
// enables each thread to have its own thread-specific logger.  The behavior of
// these methods reflects the historical two-stage release of logger
// functionality.
//
///Category Registry Capacity
///--------------------------
// The category registry capacity is managed by the 'setMaxNumCategories' and
// 'maxNumCategories' methods, and the following data member:
//..
//  unsigned int ball::LoggerManager::d_maxNumCategoriesMinusOne;
//..
// From the client's perspective, valid capacity values are in the range
// '[0 .. INT_MAX]'.  A value of 0 implies that no limit is imposed.  Capacity
// values stored internally in 'd_maxNumCategoriesMinusOne' are in the range
// '[-1 .. INT_MAX - 1]' (i.e., stored in an 'unsigned int' variable).  Note
// that 'setMaxNumCategories' and 'maxNumCategories' compensate by subtracting
// 1 from 'd_maxNumCategoriesMinusOne' and adding 1 to
// 'd_maxNumCategoriesMinusOne', respectively.  This trick allows for more
// efficient capacity testing, e.g.:
//..
//  if (d_maxNumCategoriesMinusOne >= d_categoryManager.length()) {
//      // there is sufficient capacity to add a new category
//      // ...
//  }
//..
// as compared to something like the following if the registry capacity were
// stored in a hypothetical 'int' data member
// 'ball::LoggerManager::d_maxNumCategories' not biased by 1:
//..
//  if (0 == d_maxNumCategories
//   || d_maxNumCategories > d_categoryManager.length()) {
//      // there is sufficient capacity to add a new category
//      // ...
//  }
//..
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace ball {

namespace {

const char *filterName(
   bsl::string                                             *filteredNameBuffer,
   const char                                              *originalName,
   const bsl::function<void(bsl::string *, const char *)>&  nameFilter)
    // If the specified category 'nameFilter' is a non-null functor, apply
    // 'nameFilter' to the specified 'originalName', store the translated
    // result in the specified 'filteredNameBuffer', and return the address of
    // the non-modifiable data of 'filteredNameBuffer'; return 'originalName'
    // otherwise (i.e., if 'nameFilter' is null).
{
    BSLS_ASSERT(filteredNameBuffer);
    BSLS_ASSERT(originalName);

    const char *name;
    if (nameFilter) {
        filteredNameBuffer->clear();
        nameFilter(filteredNameBuffer, originalName);
        filteredNameBuffer->push_back(0);  // append the null 'char'
        name = filteredNameBuffer->c_str();
    }
    else {
        name = originalName;
    }
    return name;
}

bool isCategoryEnabled(ball::ThresholdAggregate *levels,
                       const ball::Category&     category,
                       int                       severity)
    // Load, into the specified 'levels', the active threshold levels for the
    // specified 'category', and return 'true' if the specified 'severity' is
    // more severe (i.e., is numerically less than) at least one of the
    // threshold levels of 'category', and 'false' otherwise.

{
    if (category.relevantRuleMask()) {
        ball::AttributeContext *context = ball::AttributeContext::getContext();
        context->determineThresholdLevels(levels, &category);
        int threshold = ball::ThresholdAggregate::maxLevel(*levels);
        return threshold >= severity;                                 // RETURN
    }
    *levels = category.thresholdLevels();
    return category.maxLevel() >= severity;
}

inline static
ball::Severity::Level convertBslsLogSeverity(bsls::LogSeverity::Enum severity)
    // Return the 'ball' log severity equivalent to the specified 'bsls' log
    // 'severity'.
{
    switch (severity) {
      case bsls::LogSeverity::e_FATAL: return ball::Severity::e_FATAL;
      case bsls::LogSeverity::e_ERROR: return ball::Severity::e_ERROR;
      case bsls::LogSeverity::e_WARN:  return ball::Severity::e_WARN;
      case bsls::LogSeverity::e_INFO:  return ball::Severity::e_INFO;
      case bsls::LogSeverity::e_DEBUG: return ball::Severity::e_DEBUG;
      case bsls::LogSeverity::e_TRACE: return ball::Severity::e_TRACE;
    }
    BSLS_ASSERT_OPT(false && "Unreachable by design");
    return ball::Severity::e_ERROR;
}

static bslmt::QLock bslsLogQLock = BSLMT_QLOCK_INITIALIZER;
    // The lock used to protect the configuration of the 'bsls::Log' callback
    // handler.  This lock prevents the logger manager singleton from being
    // destroyed concurrently with an attempt to log a 'bsls::Log' record.

static bsls::Log::LogMessageHandler savedBslsLogMessageHandler = 0;
    // This variable is used to save the 'bsls::Log' message handler that is in
    // effect prior to creating the singleton, so that it can be restored when
    // the singleton is destroyed.

static ball::Category *bslsLogCategoryPtr = 0;
    // Address of the category to which 'bsls::Log' messages are logged when
    // the logger manager singleton exists.

const char *const k_BSLS_LOG_CATEGORY_NAME = "BSLS.LOG";

void bslsLogMessage(bsls::LogSeverity::Enum  severity,
                    const char              *fileName,
                    int                      lineNumber,
                    const char              *message)
    // Write a 'ball' record having the specified 'severity', 'fileName',
    // 'lineNumber', and 'message'.  Note that this function signature matches
    // 'bsls::Log::LogMessageHandler' and is intended to be installed as a
    // 'bsls::Log' message handler.
{
    bslmt::QLockGuard qLockGuard(&bslsLogQLock);

    if (ball::LoggerManager::isInitialized()) {
        ball::LoggerManager& singleton = ball::LoggerManager::singleton();

        if (0 == bslsLogCategoryPtr) {
            singleton.addCategory(k_BSLS_LOG_CATEGORY_NAME,
                                  singleton.defaultRecordThresholdLevel(),
                                  singleton.defaultPassThresholdLevel(),
                                  singleton.defaultTriggerThresholdLevel(),
                                  singleton.defaultTriggerAllThresholdLevel());

            // It is *possible* that the category "BSLS.LOG" was already added
            // (e.g., in client code), in which case the above 'addCategory'
            // would return 0.

            bslsLogCategoryPtr = singleton.lookupCategory(
                                                     k_BSLS_LOG_CATEGORY_NAME);
        }

        ball::Logger&  logger = singleton.getLogger();
        ball::Record  *record = logger.getRecord(fileName, lineNumber);

        ball::RecordAttributes& attributes = record->fixedFields();
        attributes.setMessage(message);

        logger.logMessage(*bslsLogCategoryPtr,
                          convertBslsLogSeverity(severity),
                          record);
    }
    else {
        // Don't call 'bsls::Log::logMessage' because this function is set as
        // the 'bsls::Log' message handler!

        (bsls::Log::platformDefaultMessageHandler)(severity,
                                                   fileName,
                                                   lineNumber,
                                                   message);
    }
}

const char *const DEFAULT_CATEGORY_NAME = "";
const char *const TRIGGER_BEGIN =
                                 "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ---";
const char *const TRIGGER_END =    "--- END RECORD DUMP CAUSED BY TRIGGER ---";
const char *const TRIGGER_ALL_BEGIN =
                             "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ALL ---";
const char *const TRIGGER_ALL_END =
                               "--- END RECORD DUMP CAUSED BY TRIGGER ALL ---";

void copyAttributesWithoutMessage(ball::Record                  *record,
                                  const ball::RecordAttributes&  srcAttribute)
    // Load into the specified 'record' all fixed fields except 'message',
    // 'fileName', and 'lineNumber' from the specified 'srcAttribute'.
{
    record->fixedFields().setTimestamp(srcAttribute.timestamp());
    record->fixedFields().setProcessID(srcAttribute.processID());
    record->fixedFields().setThreadID(srcAttribute.threadID());
    record->fixedFields().setCategory(srcAttribute.category());
    record->fixedFields().setSeverity(srcAttribute.severity());
}

const char *const INTERNAL_OBSERVER_NAME = "__oBsErVeR__";

}  // close unnamed namespace

                           // ------------
                           // class Logger
                           // ------------

// PRIVATE CREATORS
Logger::Logger(const bsl::shared_ptr<Observer>&            observer,
               RecordBuffer                               *recordBuffer,
               const Logger::UserFieldsPopulatorCallback&  populator,
               const PublishAllTriggerCallback&            publishAllCallback,
               int                                         scratchBufferSize,
               LoggerManagerConfiguration::LogOrder        logOrder,
               LoggerManagerConfiguration::TriggerMarkers  triggerMarkers,
               bslma::Allocator                           *globalAllocator)
: d_recordPool(-1, globalAllocator)
, d_observer(observer)
, d_recordBuffer_p(recordBuffer)
, d_populator(populator)
, d_publishAll(publishAllCallback)
, d_scratchBufferSize(scratchBufferSize)
, d_logOrder(logOrder)
, d_triggerMarkers(triggerMarkers)
, d_allocator_p(globalAllocator)
{
    BSLS_ASSERT(d_observer);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_allocator_p);

    // 'snprintf' message buffer
    d_scratchBuffer_p = (char *)d_allocator_p->allocate(d_scratchBufferSize);
}

Logger::~Logger()
{
    BSLS_ASSERT(d_observer);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_publishAll);
    BSLS_ASSERT(d_scratchBuffer_p);
    BSLS_ASSERT(d_allocator_p);

    d_observer->releaseRecords();
    d_recordBuffer_p->removeAll();
    d_allocator_p->deallocate(d_scratchBuffer_p);
}

// PRIVATE MANIPULATORS
void Logger::logMessage(const Category&            category,
                        int                        severity,
                        Record                    *record,
                        const ThresholdAggregate&  levels)
{
    record->fixedFields().setTimestamp(bdlt::CurrentTime::utc());

    record->fixedFields().setCategory(category.categoryName());
    record->fixedFields().setSeverity(severity);

    static int pid = bdls::ProcessUtil::getProcessId();
    record->fixedFields().setProcessID(pid);

    record->fixedFields().setThreadID(bslmt::ThreadUtil::selfIdAsUint64());

    if (d_populator) {
        d_populator(&record->customFields());
    }

    bsl::shared_ptr<Record> handle(record, &d_recordPool, d_allocator_p);

    if (levels.recordLevel() >= severity) {
        d_recordBuffer_p->pushBack(handle);
    }

    if (levels.passLevel() >= severity) {

        // Publish this record.

        d_observer->publish(handle,
                            Context(Transmission::e_PASSTHROUGH,
                                    0,                 // recordIndex
                                    1));               // sequenceLength

    }

    typedef LoggerManagerConfiguration Config;

    if (levels.triggerLevel() >= severity) {

        // Print markers around the trigger logs if configured.

        if (Config::e_BEGIN_END_MARKERS == d_triggerMarkers) {
            Context triggerContext(Transmission::e_TRIGGER, 0, 1);
            Record *marker = getRecord(record->fixedFields().fileName(),
                                       record->fixedFields().lineNumber());

            bsl::shared_ptr<Record> handle(marker,
                                           &d_recordPool,
                                           d_allocator_p);

            copyAttributesWithoutMessage(handle.get(), record->fixedFields());

            handle->fixedFields().setMessage(TRIGGER_BEGIN);

            d_observer->publish(handle, triggerContext);

            // Publish all records archived by *this* logger.

            publish(Transmission::e_TRIGGER);

            handle->fixedFields().setMessage(TRIGGER_END);

            d_observer->publish(handle, triggerContext);
        }
        else {
            // Publish all records archived by *this* logger.

            publish(Transmission::e_TRIGGER);
        }
    }

    if (levels.triggerAllLevel() >= severity) {

        // Print markers around the trigger logs if configured.

        if (Config::e_BEGIN_END_MARKERS == d_triggerMarkers) {
            Context triggerContext(Transmission::e_TRIGGER, 0, 1);
            Record *marker = getRecord(record->fixedFields().fileName(),
                                       record->fixedFields().lineNumber());

            bsl::shared_ptr<Record> handle(marker,
                                           &d_recordPool,
                                           d_allocator_p);

            copyAttributesWithoutMessage(handle.get(), record->fixedFields());

            handle->fixedFields().setMessage(TRIGGER_ALL_BEGIN);

            d_observer->publish(handle, triggerContext);

            // Publish all records archived by *all* loggers.

            d_publishAll(Transmission::e_TRIGGER_ALL);

            handle->fixedFields().setMessage(TRIGGER_ALL_END);

            d_observer->publish(handle, triggerContext);
        }
        else {
            // Publish all records archived by *all* loggers.

            d_publishAll(Transmission::e_TRIGGER_ALL);
        }
    }
}

void Logger::publish(Transmission::Cause cause)
{
    d_recordBuffer_p->beginSequence();

    const int len = d_recordBuffer_p->length();
    Context   context(cause, 0, len);

    if (1 == len) {  // for len == 1, order does not matter, so optimize it
        context.setRecordIndexRaw(0);
        d_observer->publish(d_recordBuffer_p->back(), context);
        d_recordBuffer_p->popBack();
    }
    else {
        if (LoggerManagerConfiguration::e_LIFO == d_logOrder) {
            for (int i = 0; i < len; ++i) {
                context.setRecordIndexRaw(i);
                d_observer->publish(d_recordBuffer_p->back(), context);
                d_recordBuffer_p->popBack();
            }
        }
        else {
            for (int i = 0; i < len; ++i) {
                context.setRecordIndexRaw(i);
                d_observer->publish(d_recordBuffer_p->front(), context);
                d_recordBuffer_p->popFront();
            }
        }
    }
    d_recordBuffer_p->endSequence();
}

// MANIPULATORS
Record *Logger::getRecord(const char *file, int line)
{
    Record *record = d_recordPool.getObject();
    record->customFields().removeAll();
    record->fixedFields().clearMessage();
    record->fixedFields().setFileName(file);
    record->fixedFields().setLineNumber(line);
    return record;
}

void Logger::logMessage(const Category&  category,
                        int              severity,
                        const char      *fileName,
                        int              lineNumber,
                        const char      *message)
{
    BSLS_ASSERT(fileName);
    BSLS_ASSERT(message);

    ThresholdAggregate thresholds(0, 0, 0, 0);
    if (!isCategoryEnabled(&thresholds, category, severity)) {
        return;                                                       // RETURN
    }

    Record *record = getRecord(fileName, lineNumber);
    record->fixedFields().setMessage(message);
    logMessage(category, severity, record, thresholds);
}

void Logger::logMessage(const Category&  category,
                        int              severity,
                        Record          *record)
{
    ThresholdAggregate thresholds(0, 0, 0, 0);
    if (!isCategoryEnabled(&thresholds, category, severity)) {
        d_recordPool.deleteObject(record);
        return;                                                       // RETURN
    }
    logMessage(category, severity, record, thresholds);
}

char *Logger::obtainMessageBuffer(bslmt::Mutex **mutex, int *bufferSize)
{
    d_scratchBufferMutex.lock();
    *mutex      = &d_scratchBufferMutex;
    *bufferSize = d_scratchBufferSize;
    return d_scratchBuffer_p;
}

                           // -------------------
                           // class LoggerManager
                           // -------------------

// CLASS DATA
LoggerManager *LoggerManager::s_singleton_p   = 0;
bool           LoggerManager::s_doNotShutDown = false;

namespace {

static bslmt::QLock singletonQLock = BSLMT_QLOCK_INITIALIZER;
    // The lock used to protect logger manager singleton (re)initialization and
    // shutdown.

}  // close unnamed namespace

// PRIVATE CLASS METHODS
void LoggerManager::initSingletonImpl(
                            const LoggerManagerConfiguration&  configuration,
                            bslma::Allocator                  *globalAllocator)
{
    bslmt::QLockGuard qLockGuard(&singletonQLock);

    if (0 == s_singleton_p) {

        bslma::Allocator *allocator =
                              bslma::Default::globalAllocator(globalAllocator);

        LoggerManager *singleton = new (*allocator) LoggerManager(
                                                                 configuration,
                                                                 allocator);
        AttributeContext::initialize(&singleton->d_categoryManager,
                                     bslma::Default::globalAllocator(0));

        s_singleton_p = singleton;

        // Configure 'bsls::Log' to publish records using 'ball' via the
        // 'LoggerManager' singleton.

        bslmt::QLockGuard qLockGuard(&bslsLogQLock);

        savedBslsLogMessageHandler = bsls::Log::logMessageHandler();
        bsls::Log::setLogMessageHandler(&bslsLogMessage);
    }
    else {
        LoggerManager::singleton().getLogger().
            logMessage(*s_singleton_p->d_defaultCategory_p,
                       Severity::e_WARN,
                       __FILE__,
                       __LINE__,
                       "BALL logger manager has already been initialized!");
    }
}

// PRIVATE CREATORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
LoggerManager::LoggerManager(
                            const LoggerManagerConfiguration&  configuration,
                            Observer                          *observer,
                            bslma::Allocator                  *globalAllocator)
: d_observer(new(*bslma::Default::globalAllocator(globalAllocator))
             BroadcastObserver(bslma::Default::globalAllocator(
                                                             globalAllocator)),
             bslma::Default::globalAllocator(globalAllocator))
, d_nameFilter(configuration.categoryNameFilterCallback())
, d_defaultThresholds(configuration.defaultThresholdLevelsCallback())
, d_defaultThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_factoryThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_populator(configuration.userFieldsPopulatorCallback())
, d_logger_p(0)
, d_categoryManager(bslma::Default::globalAllocator(globalAllocator))
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_loggers(bslma::Default::globalAllocator(globalAllocator))
, d_recordBuffer_p(0)
, d_defaultCategory_p(0)
, d_scratchBufferSize(configuration.defaults().defaultLoggerBufferSize())
, d_defaultLoggers(bslma::Default::globalAllocator(globalAllocator))
, d_logOrder(configuration.logOrder())
, d_triggerMarkers(configuration.triggerMarkers())
, d_allocator_p(bslma::Default::globalAllocator(globalAllocator))
{
    BSLS_ASSERT(d_observer);

    BSLS_ASSERT(observer);
    bsl::shared_ptr<Observer> observerWrapper(observer,
                                              bslstl::SharedPtrNilDeleter(),
                                              d_allocator_p);

    constructObject(configuration);

    int rc = registerObserver(observerWrapper, INTERNAL_OBSERVER_NAME);
    BSLS_ASSERT(0 == rc);
    (void)rc;
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

// PRIVATE MANIPULATORS
void LoggerManager::constructObject(
                               const LoggerManagerConfiguration& configuration)
{
    BSLS_ASSERT(0 == d_logger_p);
    BSLS_ASSERT(0 == d_defaultCategory_p);
    BSLS_ASSERT(0 == d_recordBuffer_p);

    d_publishAllCallback = bsl::function<void(Transmission::Cause)>(
      bsl::allocator_arg_t(),
      bsl::allocator<bsl::function<void(Transmission::Cause)> >(d_allocator_p),
      bdlf::MemFnUtil::memFn(&LoggerManager::publishAllImp, this));

    int recordBufferSize = configuration.defaults().defaultRecordBufferSize();
    d_recordBuffer_p     = new(*d_allocator_p) FixedSizeRecordBuffer(
                                                              recordBufferSize,
                                                              d_allocator_p);

    d_logger_p = new(*d_allocator_p) Logger(d_observer,
                                            d_recordBuffer_p,
                                            d_populator,
                                            d_publishAllCallback,
                                            d_scratchBufferSize,
                                            d_logOrder,
                                            d_triggerMarkers,
                                            d_allocator_p);
    d_loggers.insert(d_logger_p);
    d_defaultCategory_p = d_categoryManager.addCategory(
                                   DEFAULT_CATEGORY_NAME,
                                   d_defaultThresholdLevels.recordLevel(),
                                   d_defaultThresholdLevels.passLevel(),
                                   d_defaultThresholdLevels.triggerLevel(),
                                   d_defaultThresholdLevels.triggerAllLevel());
}

void LoggerManager::publishAllImp(Transmission::Cause cause)
{
    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    bsl::set<Logger *>::iterator itr;
    for (itr = d_loggers.begin(); itr != d_loggers.end(); ++itr) {
        (*itr)->publish(cause);
    }
}

// CLASS METHODS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
void
LoggerManager::createLoggerManager(
                             bslma::ManagedPtr<LoggerManager>  *manager,
                             Observer                          *observer,
                             const LoggerManagerConfiguration&  configuration,
                             bslma::Allocator                  *basicAllocator)
{
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    manager->load(new(*allocator) LoggerManager(configuration,
                                                observer,
                                                allocator),
                  allocator);
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

void
LoggerManager::createLoggerManager(
                             bslma::ManagedPtr<LoggerManager>  *manager,
                             const LoggerManagerConfiguration&  configuration,
                             bslma::Allocator                  *basicAllocator)
{
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    manager->load(new(*allocator) LoggerManager(configuration, allocator),
                  allocator);
}

Record *LoggerManager::getRecord(const char *file, int line)
{
    // This static method is called to obtain a record when the logger manager
    // singleton is not available (either has not been initialized or has been
    // destroyed).  The memory for the record is therefore supplied by the
    // currently installed default allocator.

    bslma::Allocator *allocator = bslma::Default::defaultAllocator();
    Record           *record    = new(*allocator) Record(allocator);

    record->fixedFields().setFileName(file);
    record->fixedFields().setLineNumber(line);
    return record;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
LoggerManager& LoggerManager::initSingleton(Observer         *observer,
                                            bslma::Allocator *globalAllocator)
{
    LoggerManagerConfiguration configuration;
    return initSingleton(observer, configuration, globalAllocator);
}

LoggerManager& LoggerManager::initSingleton(
                            Observer                          *observer,
                            const LoggerManagerConfiguration&  configuration,
                            bslma::Allocator                  *globalAllocator)
{
    BSLS_ASSERT(observer);

    // Make shared pointer with nil deleter.
    bsl::shared_ptr<Observer> observerWrapper(
                             observer,
                             bslstl::SharedPtrNilDeleter(),
                             bslma::Default::globalAllocator(globalAllocator));

    initSingletonImpl(configuration, globalAllocator);

    // Note that this call can fail if we call 'initSingleton' more than once.
    s_singleton_p->registerObserver(observerWrapper, INTERNAL_OBSERVER_NAME);

    // But we check that there is an observer registered under internal name.
    BSLS_ASSERT(s_singleton_p->findObserver(INTERNAL_OBSERVER_NAME));

    return *s_singleton_p;
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

LoggerManager& LoggerManager::initSingleton(bslma::Allocator *globalAllocator)
{
    LoggerManagerConfiguration configuration;
    return initSingleton(configuration, globalAllocator);
}

LoggerManager& LoggerManager::initSingleton(
                            const LoggerManagerConfiguration&  configuration,
                            bslma::Allocator                  *globalAllocator)
{
    initSingletonImpl(configuration, globalAllocator);
    return *s_singleton_p;
}

void
LoggerManager::initSingleton(LoggerManager *singleton, bool shutDownEnabled)
{
    BSLS_ASSERT(singleton);

    bslmt::QLockGuard qLockGuard(&singletonQLock);

    if (0 == s_singleton_p) {
        s_singleton_p   = singleton;
        s_doNotShutDown = !shutDownEnabled;

        // Configure 'bsls::Log' to publish records using 'ball' via the
        // 'LoggerManager' singleton.

        bslmt::QLockGuard qLockGuard(&bslsLogQLock);

        savedBslsLogMessageHandler = bsls::Log::logMessageHandler();
        bsls::Log::setLogMessageHandler(&bslsLogMessage);
    }
    else {
        LoggerManager::singleton().getLogger().
            logMessage(*s_singleton_p->d_defaultCategory_p,
                       Severity::e_WARN,
                       __FILE__,
                       __LINE__,
                       "BALL logger manager has already been initialized!");
    }
}

void LoggerManager::logMessage(int severity, Record *record)
{
    bsl::ostringstream datetimeStream;
    datetimeStream << bdlt::CurrentTime::utc();

    static int pid = bdls::ProcessUtil::getProcessId();

    Severity::Level severityLevel = (Severity::Level)severity;

    bsl::fprintf(stderr,
                 "\n%s %d %llu %s %s %d %s ",
                 datetimeStream.str().c_str(),
                 pid,
                 bslmt::ThreadUtil::selfIdAsUint64(),
                 Severity::toAscii(severityLevel),
                 record->fixedFields().fileName(),
                 record->fixedFields().lineNumber(),
                 "UNINITIALIZED_LOGGER_MANAGER");

    bslstl::StringRef message = record->fixedFields().messageRef();
    bsl::fwrite(message.data(), 1, message.length(), stderr);

    bsl::fprintf(stderr, "\n");

    // This static method is called to log a message when the logger manager
    // singleton is not available (either has not been initialized or has been
    // destroyed).  The memory for the record, supplied by the currently
    // installed default allocator, must therefore be reclaimed properly.

    Record::deleteObject(record);
}

char *LoggerManager::obtainMessageBuffer(bslmt::Mutex **mutex,
                                         int           *bufferSize)
{
    const int   DEFAULT_LOGGER_BUFFER_SIZE = 8192;
    static char buffer[DEFAULT_LOGGER_BUFFER_SIZE];

    static bsls::ObjectBuffer<bslmt::Mutex> staticMutex;
    BSLMT_ONCE_DO {
        // This mutex must remain valid for the lifetime of the task, and is
        // intentionally never destroyed.  This function may be called on
        // program termination, e.g., if a statically initialized object
        // performs logging during its destruction.

        new (staticMutex.buffer()) bslmt::Mutex();
    }

    staticMutex.object().lock();
    *mutex      = &staticMutex.object();
    *bufferSize = DEFAULT_LOGGER_BUFFER_SIZE;

    return buffer;
}

void LoggerManager::shutDownSingleton()
{
    bslmt::QLockGuard qLockGuard(&singletonQLock);

    if (s_singleton_p && !s_doNotShutDown) {
        // Restore the 'bsls::Log' message handler that was in effect prior to
        // the creation of the singleton.  The lock ensures that the singleton
        // is not destroyed while a 'bsls::Log' record is being published.

        bslmt::QLockGuard qLockGuard(&bslsLogQLock);
        bsls::Log::setLogMessageHandler(savedBslsLogMessageHandler);

        // Clear the singleton pointer as early as possible to narrow the
        // window during which one thread is destroying the singleton while
        // another is still accessing its data members.

        LoggerManager *singleton = s_singleton_p;

        s_singleton_p      = 0;
        bslsLogCategoryPtr = 0;

        AttributeContext::reset();

        singleton->allocator()->deleteObjectRaw(singleton);
    }
}

// CREATORS
LoggerManager::LoggerManager(
                            const LoggerManagerConfiguration&  configuration,
                            bslma::Allocator                  *globalAllocator)
: d_observer(new(*bslma::Default::globalAllocator(globalAllocator))
             BroadcastObserver(bslma::Default::globalAllocator(
                                                             globalAllocator)),
             bslma::Default::globalAllocator(globalAllocator))
, d_nameFilter(configuration.categoryNameFilterCallback())
, d_defaultThresholds(configuration.defaultThresholdLevelsCallback())
, d_defaultThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_factoryThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_populator(configuration.userFieldsPopulatorCallback())
, d_logger_p(0)
, d_categoryManager(bslma::Default::globalAllocator(globalAllocator))
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_loggers(bslma::Default::globalAllocator(globalAllocator))
, d_recordBuffer_p(0)
, d_defaultCategory_p(0)
, d_scratchBufferSize(configuration.defaults().defaultLoggerBufferSize())
, d_defaultLoggers(bslma::Default::globalAllocator(globalAllocator))
, d_logOrder(configuration.logOrder())
, d_triggerMarkers(configuration.triggerMarkers())
, d_allocator_p(bslma::Default::globalAllocator(globalAllocator))
{
    BSLS_ASSERT(d_observer);

    constructObject(configuration);
}

LoggerManager::~LoggerManager()
{
    BSLS_ASSERT(d_logger_p);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_defaultCategory_p);
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT( 0 < d_loggers.size());
    BSLS_ASSERT(-1 <= (int)d_maxNumCategoriesMinusOne);

    // To minimize the chance that one thread is destroying the singleton
    // while another is still accessing the data members, immediately reset
    // all category holders to their default value.

    d_observer->deregisterAllObservers();

    d_categoryManager.resetCategoryHolders();  // do this first!

    bsl::set<Logger *>::iterator itr;
    for (itr = d_loggers.begin(); itr != d_loggers.end(); ++itr) {
        (*itr)->~Logger();
        d_allocator_p->deallocate(*itr);
    }
    d_recordBuffer_p->~RecordBuffer();
    d_allocator_p->deallocate(d_recordBuffer_p);
}

// MANIPULATORS
                             // Logger Management

Logger *LoggerManager::allocateLogger(RecordBuffer *buffer)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(d_observer,
                                                buffer,
                                                d_populator,
                                                d_publishAllCallback,
                                                d_scratchBufferSize,
                                                d_logOrder,
                                                d_triggerMarkers,
                                                d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

Logger *LoggerManager::allocateLogger(RecordBuffer *buffer,
                                      int           scratchBufferSize)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(d_observer,
                                                buffer,
                                                d_populator,
                                                d_publishAllCallback,
                                                scratchBufferSize,
                                                d_logOrder,
                                                d_triggerMarkers,
                                                d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
Logger *LoggerManager::allocateLogger(RecordBuffer *buffer,
                                      Observer     *observer)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    bsl::shared_ptr<Observer> observerWrapper(observer,
                                              bslstl::SharedPtrNilDeleter(),
                                              d_allocator_p);

    Logger *logger = new(*d_allocator_p) Logger(observerWrapper,
                                                buffer,
                                                d_populator,
                                                d_publishAllCallback,
                                                d_scratchBufferSize,
                                                d_logOrder,
                                                d_triggerMarkers,
                                                d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

Logger *LoggerManager::allocateLogger(RecordBuffer *buffer,
                                      int           scratchBufferSize,
                                      Observer     *observer)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    bsl::shared_ptr<Observer> observerWrapper(observer,
                                              bslstl::SharedPtrNilDeleter(),
                                              d_allocator_p);

    Logger *logger = new(*d_allocator_p) Logger(observerWrapper,
                                                buffer,
                                                d_populator,
                                                d_publishAllCallback,
                                                scratchBufferSize,
                                                d_logOrder,
                                                d_triggerMarkers,
                                                d_allocator_p);

    d_loggers.insert(logger);

    return logger;
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

Logger *LoggerManager::allocateLogger(
                                    RecordBuffer                     *buffer,
                                    const bsl::shared_ptr<Observer>&  observer)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(observer,
                                                buffer,
                                                d_populator,
                                                d_publishAllCallback,
                                                d_scratchBufferSize,
                                                d_logOrder,
                                                d_triggerMarkers,
                                                d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

Logger *LoggerManager::allocateLogger(
                           RecordBuffer                     *buffer,
                           int                               scratchBufferSize,
                           const bsl::shared_ptr<Observer>&  observer)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(observer,
                                                buffer,
                                                d_populator,
                                                d_publishAllCallback,
                                                scratchBufferSize,
                                                d_logOrder,
                                                d_triggerMarkers,
                                                d_allocator_p);

    d_loggers.insert(logger);

    return logger;
}

void LoggerManager::deallocateLogger(Logger *logger)
{
    d_loggersLock.lockWrite();
    d_loggers.erase(logger);
    d_loggersLock.unlock();

    d_defaultLoggersLock.lockWrite();

    bsl::map<void *, Logger *>::iterator itr = d_defaultLoggers.begin();
    while (itr != d_defaultLoggers.end()) {
        if (logger == itr->second) {
            d_defaultLoggers.erase(itr++);
        }
        else {
            ++itr;
        }
    }
    d_defaultLoggersLock.unlock();

    logger->~Logger();
    d_allocator_p->deallocate(logger);
}

Logger& LoggerManager::getLogger()
{
    // TBD: optimize it using thread local storage
    d_defaultLoggersLock.lockRead();
    bsl::map<void *, Logger *>::iterator itr =
            d_defaultLoggers.find((void *)bslmt::ThreadUtil::selfIdAsUint64());
    d_defaultLoggersLock.unlock();
    return itr != d_defaultLoggers.end() ? *(itr->second) : *d_logger_p;
}

void LoggerManager::setLogger(Logger *logger)
{
    void *id = (void *)bslmt::ThreadUtil::selfIdAsUint64();

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(
                                                        &d_defaultLoggersLock);
    if (0 == logger) {
        d_defaultLoggers.erase(id);
    }
    else {
        d_defaultLoggers[id] = logger;
    }
}

                             // Category Management

Category *LoggerManager::addCategory(const char *categoryName,
                                     int         recordLevel,
                                     int         passLevel,
                                     int         triggerLevel,
                                     int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    Category *category = d_categoryManager.lookupCategory(localCategoryName);
    if (category) {
        return 0;                                                     // RETURN
    }

    if (d_maxNumCategoriesMinusOne <
                                   (unsigned int) d_categoryManager.length()) {
        return 0;                                                     // RETURN
    }

    return d_categoryManager.addCategory(localCategoryName,
                                         recordLevel,
                                         passLevel,
                                         triggerLevel,
                                         triggerAllLevel);
}

Category *LoggerManager::lookupCategory(const char *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    return d_categoryManager.lookupCategory(filterName(&filteredName,
                                                       categoryName,
                                                       d_nameFilter));
}

const Category *LoggerManager::setCategory(CategoryHolder *categoryHolder,
                                           const char     *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    Category *category = d_categoryManager.lookupCategory(categoryHolder,
                                                          localCategoryName);

    if (!category
     && d_maxNumCategoriesMinusOne >=
                                   (unsigned int) d_categoryManager.length()) {
        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        {
            bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> guard(
                                                     &d_defaultThresholdsLock);
            if (d_defaultThresholds) {
                d_defaultThresholds(&recordLevel,
                                    &passLevel,
                                    &triggerLevel,
                                    &triggerAllLevel,
                                    localCategoryName);
            }
            else {
                recordLevel     = d_defaultThresholdLevels.recordLevel();
                passLevel       = d_defaultThresholdLevels.passLevel();
                triggerLevel    = d_defaultThresholdLevels.triggerLevel();
                triggerAllLevel = d_defaultThresholdLevels.triggerAllLevel();
            }
        }

        category = d_categoryManager.addCategory(categoryHolder,
                                                 localCategoryName,
                                                 recordLevel,
                                                 passLevel,
                                                 triggerLevel,
                                                 triggerAllLevel);

        if (!category) {  // added by another thread?
            category = d_categoryManager.lookupCategory(categoryHolder,
                                                        localCategoryName);
        }
    }

    return category ? category
                    : d_categoryManager.lookupCategory(categoryHolder,
                                                       DEFAULT_CATEGORY_NAME);
}

Category *LoggerManager::setCategory(const char *categoryName,
                                     int         recordLevel,
                                     int         passLevel,
                                     int         triggerLevel,
                                     int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    Category *category = d_categoryManager.lookupCategory(localCategoryName);

    if (category) {
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);
        return category;                                              // RETURN
    }

    if (d_maxNumCategoriesMinusOne <
                                   (unsigned int) d_categoryManager.length()) {
        return 0;                                                     // RETURN
    }

    category = d_categoryManager.addCategory(localCategoryName,
                                             recordLevel,
                                             passLevel,
                                             triggerLevel,
                                             triggerAllLevel);

    if (category) {
        return category;                                              // RETURN
    }
    else {
        return d_categoryManager.lookupCategory(localCategoryName);   // RETURN
    }
}

void LoggerManager::setMaxNumCategories(int length)
{
    d_maxNumCategoriesMinusOne = length - 1;
}

                             // Observer Management

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
Observer *LoggerManager::observer()
{
    return findObserver(INTERNAL_OBSERVER_NAME).get();
}
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

                             // Threshold Level Management

void LoggerManager::setCategoryThresholdsToCurrentDefaults(Category *category)
{
    category->setLevels(d_defaultThresholdLevels.recordLevel(),
                        d_defaultThresholdLevels.passLevel(),
                        d_defaultThresholdLevels.triggerLevel(),
                        d_defaultThresholdLevels.triggerAllLevel());
}

void LoggerManager::setCategoryThresholdsToFactoryDefaults(Category *category)
{
    category->setLevels(d_factoryThresholdLevels.recordLevel(),
                        d_factoryThresholdLevels.passLevel(),
                        d_factoryThresholdLevels.triggerLevel(),
                        d_factoryThresholdLevels.triggerAllLevel());
}

int LoggerManager::setDefaultThresholdLevels(int recordLevel,
                                             int passLevel,
                                             int triggerLevel,
                                             int triggerAllLevel)
{
    enum { BALL_SUCCESS = 0, BALL_FAILURE = -1 };

    if (!Category::areValidThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel)) {
        return BALL_FAILURE;                                          // RETURN
    }

    d_defaultThresholdLevels.setLevels(recordLevel,
                                       passLevel,
                                       triggerLevel,
                                       triggerAllLevel);
    return BALL_SUCCESS;
}

void LoggerManager::setDefaultThresholdLevelsCallback(
                                      DefaultThresholdLevelsCallback *callback)
{
    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> guard(
                                                     &d_defaultThresholdsLock);
    if (callback) {
        d_defaultThresholds = *callback;
    }
    else {
        DefaultThresholdLevelsCallback nullCallback;
        d_defaultThresholds = nullCallback;
    }
}

// ACCESSORS
bool LoggerManager::isCategoryEnabled(const Category *category,
                                      int             severity) const
{
    if (category->relevantRuleMask()) {
        AttributeContext *context = AttributeContext::getContext();
        ThresholdAggregate levels(0, 0, 0, 0);
        context->determineThresholdLevels(&levels, category);
        int threshold = ThresholdAggregate::maxLevel(levels);
        return threshold >= severity;                                 // RETURN
    }
    return category->maxLevel() >= severity;
}

const Category *LoggerManager::lookupCategory(const char *categoryName) const
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    return d_categoryManager.lookupCategory(filterName(&filteredName,
                                                       categoryName,
                                                       d_nameFilter));
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
const Observer *LoggerManager::observer() const
{
    return findObserver(INTERNAL_OBSERVER_NAME).get();
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

const Logger::UserFieldsPopulatorCallback *
LoggerManager::userFieldsPopulatorCallback() const
{
    return d_populator ? &d_populator : 0;
}

}  // close package namespace
}  // close enterprise namespace

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
