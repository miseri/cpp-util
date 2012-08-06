#pragma once

#include <boost/cstdint.hpp>
#include <boost/shared_array.hpp>

/**
 * Wrapper class around boost::shared array
 */
class Buffer
{
public:
  typedef boost::shared_array< uint8_t > DataBuffer_t;

  Buffer()
    :m_buffer( DataBuffer_t() ),
    m_uiSize( 0 )
  {}

  Buffer(uint8_t* ptr, size_t size)
    :m_buffer( DataBuffer_t(ptr) ),
    m_uiSize(size)
  {}

  ~Buffer()
  {

  }

  uint8_t& operator[](std::ptrdiff_t i) const
  {
    return m_buffer[i];
  }

  DataBuffer_t& getBuffer() { return m_buffer; }
  size_t getSize() const { return m_uiSize; }

  void setData(uint8_t* ptr, size_t size)
  {
    m_buffer.reset(ptr);
    m_uiSize = size;
  }

  void reset()
  {
    m_buffer.reset();
    m_uiSize = 0;
  }

  const uint8_t* data() const
  {
    return m_buffer.get();
  }

  std::string toStdString() const
  {
    return std::string( (char*)m_buffer.get(), m_uiSize);
  }

private:
  DataBuffer_t m_buffer;
  size_t m_uiSize;
};

