#pragma once

#include <memory>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>

// -DSINGLE_CORE: can be used to simplify debugging and determining
// whether multi-threading related bugs are occurring
#define SINGLE_CORE

/**
 * This class provides active object functionality
 * Sub-classes should provide event-loop starting and 
 * stopping functions by overriding doStart and doStop
 * doPeriodicTask allows subclasses to execute periodic tasks
 * based on the interval specified in the contructor
 */
class ServiceController : public boost::noncopyable
{
  public:
    virtual ~ServiceController(){}

    boost::asio::io_service& getIoService()
    {
      return m_rIo_service;
    }

    bool isRunning() const { return m_eState == SS_RUNNING; }
    bool isReady() const { return m_eState == SS_READY; }
    bool isStopping() const { return m_eState == SS_STOPPING; }

    boost::system::error_code start()
    {
      // give subclass a chance to take action
      boost::system::error_code ec = doStart();
      if (ec) return ec;

      m_pWork = boost::shared_ptr<boost::asio::io_service::work>(new boost::asio::io_service::work(m_rIo_service));
      m_timer.async_wait(boost::bind(&ServiceController::onTimer, this, boost::asio::placeholders::error ));

      unsigned uiCores = boost::thread::hardware_concurrency();
      LOG(INFO) << "Got " << uiCores << " cores";

      uiCores = (uiCores > 0) ? uiCores : 1;

#ifdef SINGLE_CORE
      uiCores = 1;
#endif
      VLOG(1) << "Using " << uiCores << " cores";

      // create threads for io service
      if (uiCores > 1)
      {
        boost::thread_group worker_threads;
        for( unsigned x = 0; x < uiCores; ++x )
        {
          worker_threads.create_thread( boost::bind( &ServiceController::runIoService, this ) );
        }
        m_eState = SS_RUNNING;
        worker_threads.join_all();
      }
      else
      {
        m_eState = SS_RUNNING;
        runIoService();
      }

      m_rIo_service.reset();
      m_eState = SS_READY;
      return boost::system::error_code();
    }

    boost::system::error_code stop()
    {
      m_eState = SS_STOPPING;
      // allow subclasses to control stopping
      doStop();
      // Stop the work: this will result in the the io_service stopping once it runs out of work
      m_pWork.reset();
      // Stop event loop if running. Else return already stopped
      m_timer.cancel();
      //m_ioService.stop();
      return boost::system::error_code();
    }

  public:
  protected:
    ServiceController(unsigned uiTimerTimeoutMs = 1000)
      :m_rIo_service(m_ioService),
      m_strand(m_rIo_service),
      m_eState(SS_READY),
      m_uiTimerTimeoutMs(uiTimerTimeoutMs),
      m_timer(m_rIo_service, boost::posix_time::milliseconds(m_uiTimerTimeoutMs))
  {

  }

    ServiceController(boost::asio::io_service& io_service, unsigned uiTimerTimeoutMs = 1000)
      :m_rIo_service(io_service),
      m_strand(m_rIo_service),
      m_eState(SS_READY),
      m_uiTimerTimeoutMs(uiTimerTimeoutMs),
      m_timer(m_rIo_service, boost::posix_time::milliseconds(m_uiTimerTimeoutMs))
  {

  }

  protected:
    virtual boost::system::error_code doStart(){  return boost::system::error_code(); }
    virtual boost::system::error_code doStop(){  return boost::system::error_code(); }
    virtual void doPeriodicTask() {}

  private:
    void onTimer( const boost::system::error_code& ec )
    {
      if (!ec)
      {
        try
        {
          doPeriodicTask();
#if 0
          VLOG(5) << "[" << boost::this_thread::get_id() << "] Do periodic tasks (" << m_uiTimerTimeoutMs << "ms)" << std::endl;
#endif
        }
        catch(boost::exception &e)
        { 
          LOG(ERROR) << "Boost Exception: " << boost::diagnostic_information(e);
        }
        catch(std::exception& e) 
        {
          LOG(ERROR) << "Std Exception: " << e.what();
        }

        /// Schedule next report 
        m_timer.expires_at(m_timer.expires_at() + boost::posix_time::milliseconds(m_uiTimerTimeoutMs));
        //m_timer.async_wait(boost::bind(&ServiceController::onTimer, this, boost::asio::placeholders::error));

        // causes hang
        //m_timer.async_wait(m_strand.wrap(boost::bind(&ServiceController::onTimer, this, boost::asio::placeholders::error)));

        m_timer.async_wait(boost::bind(&ServiceController::onTimer, this, boost::asio::placeholders::error));
      }
      else
      {
        if (ec != boost::asio::error::operation_aborted)
        {
          LOG(WARNING) << "Error: " << ec.message();
        }
        else
        {
          VLOG(1) << "Shutting down";
        }
      }
    }

    void runIoService()
    {
      try
      {
        VLOG(1) << "[" << boost::this_thread::get_id() << "] Running io service thread";
        m_rIo_service.run();
        VLOG(1) << "[" << boost::this_thread::get_id() << "] End of io service thread";
        return;
      }
      catch(boost::exception &e)
      { 
        LOG(ERROR) << "Boost Exception: " << boost::diagnostic_information(e);
      }
      catch(std::exception& e) 
      {
        LOG(ERROR) << "Boost Exception: " << boost::diagnostic_information(e);
      }
      LOG(WARNING) << "[" << boost::this_thread::get_id() << "] End of io service thread due to exception";
    }


  private:
    boost::asio::io_service m_ioService;
  protected:
    boost::asio::io_service& m_rIo_service;
  private:
    boost::shared_ptr<boost::asio::io_service::work> m_pWork;
    boost::asio::io_service::strand m_strand;

    enum ServiceState
    {
      SS_READY,
      SS_RUNNING,
      SS_STOPPING
    };
    ServiceState m_eState;

    unsigned m_uiTimerTimeoutMs;
    boost::asio::deadline_timer m_timer;

};

