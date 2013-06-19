#pragma once

#include <boost/exception_ptr.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <glog/logging.h>

typedef boost::function<void (boost::system::error_code)> CompletionHandler_t;

template <class T>
struct ServiceTraits
{
  static const char* name;
};

namespace details {
  template <typename T>
  struct ServiceDeducer {
    typedef boost::optional<T> type;
  };

  template <typename T>
  struct ServiceDeducer< T* > {
    typedef T* type;
  };

  template <typename T>
  struct ServiceDeducer< boost::shared_ptr<T> > {
    typedef boost::shared_ptr<T> type;
  };
} // namespace details

/// classes using this template can specialize this trait to customize the name
template <class T>
const char* ServiceTraits<T>::name = "ServiceName";

/// Templatized Asynchronous Service Class
/// This class handles the thread management and lifetime and allows the caller 
/// to create an non-blocking service by meeting the service implementation interface requirements.
/// The Service implementation must provide the following interface:
/// boost::system::error_code onStart(): 
/// - This method can be used to perform any pre-service requirements and runs in the calling thread
/// - If an error code is returned, the service will not be launched!
/// boost::system::error_code start():
/// - This method is run in the new thread. The implementation would typically provide some kind of event loop in this method
/// boost::system::error_code stop(): 
/// - This method is run in the calling thread and MUST terminate the event loop running in the service thread
/// boost::system::error_code onComplete(): 
/// - This method is run in the calling thread and can be used to cleanup resources, etc

/*
  Implementation interface:
  boost::system::error_code onStart(){}
  boost::system::error_code start(){}
  boost::system::error_code stop(){}
  boost::system::error_code onComplete(){} 
*/

template <typename T>
class ServiceThread : private boost::noncopyable
{
  typedef typename details::ServiceDeducer<T>::type ServiceImpl;
  typedef boost::shared_ptr<boost::thread> thread_ptr;

public:
  typedef boost::shared_ptr<ServiceThread<T> > ptr;

  ServiceThread()
    :m_eState(SS_READY),
    m_sServiceName(ServiceTraits<T>::name)
  {
    VLOG(10) << "Service created: " << m_sServiceName;
  }

  ServiceThread(ServiceImpl impl)
    :m_eState(SS_READY),
    m_sServiceName(ServiceTraits<T>::name),
    m_pImpl(impl)
  {
    VLOG(10) << "Service created: " << m_sServiceName;
  }

  ~ServiceThread()
  {
    VLOG(10) << "Service destructor: " << m_sServiceName;

    /// try stop the service in case it's running
    if (m_pServiceThread && m_pServiceThread->joinable())
    {
      stop();
    }
    VLOG(10) << "Service complete: " << m_sServiceName;
  }

  static ptr create()
  {
    return boost::make_shared<ServiceThread<T> >();
  }

  bool isRunning() const { return m_eState == SS_RUNNING; }
  bool isReady() const { return m_eState == SS_READY; }
  bool isStopping() const { return m_eState == SS_STOPPING; }

  /// Accessor to service implementation. The handle can be used to configure the implementation object
  ServiceImpl& get() { return m_pImpl; }

  /// Mutator to service implementation. The handle can be used to configure the implementation object
  void set(ServiceImpl serviceImpl)
  {
    // the implementation object cannot be modified once the thread has been created
    assert(m_pServiceThread == 0);
    m_pImpl = serviceImpl;
  }

  /// Completion handler
  void setCompletionHandler(CompletionHandler_t onComplete) { m_onComplete = onComplete; }

  /// if the service implementation reports an error from the start or stop method call, it can be accessed via this method
  /// NB: only the last error can be accessed
  boost::system::error_code getServiceErrorCode() const { return m_ecService; }

  /// The join method allows the caller to block until thread completion
  void join()
  {
    // protect this method from being called twice (e.g. by user and by stop)
    boost::mutex::scoped_lock lock(m_joinMutex);
    if (m_pServiceThread && m_pServiceThread->joinable())
    {
      VLOG(10) << "Joining: " << m_sServiceName;
      m_pServiceThread->join();
      VLOG(10) << "Resetting handle: " << m_sServiceName;
      m_pServiceThread.reset();
    }
  }

  /// This method launches the non-blocking service
  boost::system::error_code start()
  {
    boost::mutex::scoped_lock lock(m_threadMutex);

    if (m_pServiceThread && m_pServiceThread->joinable())
    {
      LOG(WARNING) << "Service already running: " << m_sServiceName;
      // already running
      return boost::system::error_code(boost::system::errc::operation_not_permitted, boost::system::get_generic_category());
    }

    boost::system::error_code ec = m_pImpl->onStart();

    if (ec)
    {
      LOG(ERROR) << "Error on service start: " << m_sServiceName << " Message: " << ec.message();
      return ec;
    }

    VLOG(10) << "Starting service: " << m_sServiceName;
    m_pServiceThread = thread_ptr(new boost::thread(boost::bind(&ServiceThread::main, this)));

    m_startCondition.wait(m_threadMutex);

    // notify main to continue: it's blocked on the same condition var
    m_startCondition.notify_one();
    m_eState = SS_READY;
    // No error
    return boost::system::error_code();
  }

  /// This method stops the non-blocking service
  boost::system::error_code stop()
  {
    VLOG(10) << "Stopping service: " << m_sServiceName;
    m_eState = SS_STOPPING;
    // trigger the stopping of the event loop
    boost::system::error_code ec = m_pImpl->stop();
    if (ec)
    {
      LOG(ERROR) << "Error on service stop: " << m_sServiceName << " Message: " << ec.message();
    }
    ec = m_pImpl->onComplete();

    if (ec)
    {
      LOG(ERROR) << "Error on service complete: " << m_sServiceName << " Message: " << ec.message();
    }
    // The service implementation can return an error code here for more information
    // However it is the responsibility of the implementation to stop the service event loop (if running)
    // Failure to do so, will result in a block
    // If this occurs in practice, we may consider a timed join?
    join();
    VLOG(10) << "Stopping complete: " << m_sServiceName;

    // If exception was thrown in new thread, rethrow it.
    // Should the template implementation class want to avoid this, it should catch the exception
    // in its start method and then return and error code instead
    if( m_exception )
      boost::rethrow_exception(m_exception);

    return ec;
  }

private:
  /// runs in it's own thread
  void main()
  {
    try
    {
      VLOG(10) << "Service thread started: " << m_sServiceName;

      boost::mutex::scoped_lock lock(m_threadMutex);
      // notify main thread that it can continue
      m_startCondition.notify_one();
      // Try Dummy wait to allow 1st thread to resume
      m_startCondition.wait(m_threadMutex);

      // call implementation of event loop
      // This will block
      // In scenarios where the service fails to start, the implementation can return an error code
      boost::system::error_code ec = m_ecService = m_pImpl->start();

      VLOG(10) << "Service thread complete: " << m_sServiceName;

      m_exception = boost::exception_ptr();

      if (m_onComplete)(m_onComplete(ec));
      m_eState = SS_READY;
    }
    catch (...)
    {
      m_exception = boost::current_exception();
      LOG(ERROR) << "Service exception: " << m_sServiceName << " Message: " << boost::diagnostic_information(m_exception);
    }
  }

  enum ServiceState
  {
    SS_READY,
    SS_RUNNING,
    SS_STOPPING
  };
  ServiceState m_eState;
  /// Service name
  std::string m_sServiceName;
  /// Service thread
  thread_ptr m_pServiceThread;
  /// Thread mutex
  mutable boost::mutex m_threadMutex;
  /// Join mutex
  mutable boost::mutex m_joinMutex;
  /// Condition for signaling start of thread
  boost::condition m_startCondition;

  /// T must satisfy the implicit service interface and provide a start and a stop method
  ServiceImpl m_pImpl;

  // Error code for service implementation errors
  boost::system::error_code m_ecService;

  // Exception ptr to transport exception across different threads
  boost::exception_ptr m_exception;

  CompletionHandler_t m_onComplete;
};
