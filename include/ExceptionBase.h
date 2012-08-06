#pragma once

#include <exception>
#include <boost/exception/all.hpp> 

#include <string>

typedef boost::error_info<struct tag_errmsg, std::string> errmsg_info; 

class ExceptionBase: public boost::exception, public std::exception
{
public:
  ExceptionBase(const std::string& sMessage)
    :m_sMessage(sMessage)
  {
  }

  ~ExceptionBase() throw ()
  {
  }

  virtual const char *what() const throw() 
  { 
    return m_sMessage.c_str(); 
  } 

  virtual const std::string message() const throw()
  {
    return m_sMessage;
  }

  void prepend(const std::string& sMessage)
  {
    m_sMessage = sMessage + m_sMessage;
  }

  void append(const std::string& sMessage)
  {
    m_sMessage += sMessage;
  }
protected:
  std::string m_sMessage;
};

