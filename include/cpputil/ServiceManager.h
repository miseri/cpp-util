#pragma once
#include <cstdint>
#include <unordered_map>
#include <cpputil/ServiceController.h>

class ServiceManager : public ServiceController
{
public:
  /// A service is defined as a start and stop function
  typedef boost::function<boost::system::error_code ()> ServiceCb_t;
  typedef std::pair<ServiceCb_t, ServiceCb_t> Service_t;

  static const uint32_t NO_ERROR_ID = UINT_MAX;


  /// Default Constructor: Service manager uses io_service of ServiceController
  ServiceManager()
    :m_uiServiceId(0),
      m_uiLastErrorServiceId(NO_ERROR_ID)
  {
    VLOG(10) << "Constructor: using own IO service";
  }

  /// Default Constructor: Service manager uses provide io_service
  ServiceManager(boost::asio::io_service& ioService)
    :ServiceController(ioService),
      m_uiServiceId(0),
      m_uiLastErrorServiceId(NO_ERROR_ID)
  {
    VLOG(10) << "Constructor: using provided IO service";
  }

  /// return service id of last error
  uint32_t getLastErrorServiceId() const { return m_uiLastErrorServiceId;}

  /// registers service IFF services are not running already.
  /// returns true if successful
  /// ID that can be used to deregister service
  bool registerService(ServiceCb_t onStart, ServiceCb_t onStop, uint32_t& uiServiceId )
  {
    if (isReady())
    {
      m_mServices[m_uiServiceId] = std::make_pair(onStart, onStop);
      uiServiceId = m_uiServiceId++;
      VLOG(10) << "Service registered: " << uiServiceId;
      return true;
    }
    else
    {
      LOG(WARNING) << "Failed to register service";
      return false;
    }
  }

  /// deregisters service IFF services are not currently runnng.
  bool deregisterService(uint32_t uiServiceId)
  {
    auto it = m_mServices.find(uiServiceId);
    if (it == m_mServices.end())
    {
      LOG(WARNING) << "Failed to deregister service";
      return false;
    }
    else
    {
      VLOG(10) << "Service deregistered: " << uiServiceId;
      m_mServices.erase(it);
      return true;
    }
  }

protected:

  /**
   * @fn  virtual boost::system::error_code ServiceManager::doStart()
   * @brief Starts all services managed by this component.
   * This method will exit if something goes wrong during start
   * and return the error code
   * @return  .
   */
  virtual boost::system::error_code doStart()
  {
    m_lastError = boost::system::error_code();
    m_uiLastErrorServiceId = NO_ERROR_ID;

    boost::system::error_code ec;
    std::vector<uint32_t> started;
    for (const std::pair<uint32_t, Service_t>& pair : m_mServices)
    {
      ec = pair.second.first();
      if (!ec)
      {
        // remember started ids so that we can stop them in case one fails
        started.push_back(pair.first);
      }
      else
      {
        LOG(WARNING) << "Failed to start service: " << ec.message();
        // failure
        m_lastError = ec;
        m_uiLastErrorServiceId = pair.first;
        break;
      }
    }

    if (m_lastError)
    {
      // stop previously started services
      for (uint32_t uiId : started)
      {
        // don't overwrite the error that causes the stop...
        boost::system::error_code ec = m_mServices[uiId].second();
        if (ec)
        {
          LOG(WARNING) << "Failed to stop service: " << ec.message();
        }
      }
    }

    return m_lastError;
  }

  /**
   * @fn  virtual boost::system::error_code ServiceManager::doStop()
   * @brief Stops all services managed by this component
   * @return  .
   */
  virtual boost::system::error_code doStop()
  {
    m_lastError = boost::system::error_code();
    m_uiLastErrorServiceId = NO_ERROR_ID;

    boost::system::error_code ec;
    for (const std::pair<uint32_t, Service_t>& pair : m_mServices)
    {
      ec = pair.second.second();
      if (ec)
      {
        LOG(WARNING) << "Failed to stop service: " << ec.message();
        m_uiLastErrorServiceId = pair.first;
        m_lastError = ec;
      }
    }

    if (m_lastError)
    {
      return m_lastError;
    }
  }

private:
  //
  uint32_t m_uiServiceId;
  std::unordered_map<uint32_t, Service_t> m_mServices;

  // store information about the last error
  boost::system::error_code m_lastError;
  uint32_t m_uiLastErrorServiceId;
};
