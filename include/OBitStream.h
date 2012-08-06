#pragma once

#include <cstring>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_array.hpp>
#include <rtp++/Buffer.h>

#define DEFAULT_BUFFER_SIZE 1024

/// DEBUG_OBITSTREAM for outputting debug info

/**
 * Class to write to a bitstream
 */
class OBitStream
{
  //typedef boost::shared_array<uint8_t> buffer_t;
public:

  OBitStream(const uint32_t uiSize = DEFAULT_BUFFER_SIZE)
    :m_uiBufferSize(uiSize),
    m_buffer(new uint8_t[uiSize], uiSize),
    m_uiBitsLeft(8),
    m_uiCurrentBytePos(0)
  {
    memset(m_buffer.getBuffer().get(), 0, m_uiBufferSize);
  }

  OBitStream(Buffer buffer)
    :m_uiBufferSize(buffer.getSize()),
    m_buffer(buffer),
    m_uiBitsLeft(8),
    m_uiCurrentBytePos(0)
  {
    memset(m_buffer.getBuffer().get(), 0, m_uiBufferSize);
  }

  void write8Bits(uint8_t uiValue)
  {
    if (totalBitsLeft() < 8)
    {
      increaseBufferSize(m_uiBufferSize << 1);
    }
    // check if we're writing on byte boundary
    if (m_uiBitsLeft == 8)
    {
      m_buffer[m_uiCurrentBytePos++] = uiValue;
    }
    else
    {
      write(uiValue, 8);
    }
  }

  void write(uint32_t uiValue, uint32_t uiBits)
  {
    // check if enough memory has been allocated
    if ( totalBitsLeft() < uiBits )
    {
      // reallocate more than enough memory:
      uint32_t uiBytes = uiBits >> 3;
      uint32_t uiNewSize = std::max(m_uiBufferSize << 1, (m_uiBufferSize + uiBytes) << 1 );
      increaseBufferSize(uiNewSize);
    }

    // check if we can do a fast copy: on byte boundaries where uiBits is a multiple of 8
    // is % maybe faster?
    if (m_uiBitsLeft == 8 && (uiBits == 8 ||  uiBits == 16 || uiBits == 24 || uiBits == 32))
    {
      // TODO: check endianness of machine
      bool bLittleEndian = true;
      if (bLittleEndian)
      {
        switch (uiBits)
        {
          case 32:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 32;
#endif
            m_buffer[m_uiCurrentBytePos++] = ((uiValue >> 24) & 0xff); 
          }
          case 24:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 24;
#endif
            m_buffer[m_uiCurrentBytePos++] = ((uiValue >> 16) & 0xff);
          }
          case 16:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 16;
#endif
            m_buffer[m_uiCurrentBytePos++] = ((uiValue >> 8) & 0xff);
          }
          case 8:
          {
#ifdef DEBUG_OBITSTREAM
            DLOG(INFO) << 8;
#endif
            m_buffer[m_uiCurrentBytePos++] = uiValue;
            break;
          }
        }
      }
      else
      {
        //memcpy(&m_buffers[m_uiCurrentBufferPos][m_uiCurrentBytePos],  );
        assert(false);
      }
    }
    else
    {
      // calculate mask: (2^bits - 1) is a mask for uiBits number of bits
      // can only write a maximum of m_uiBitsLeft at a time
      uint32_t uiBitsLeftToWrite = uiBits;
      uint32_t uiValueCopy = uiValue;
      while (uiBitsLeftToWrite)
      {
        // write max of m_uiBitsLeft bits
        uint8_t uiBitsToWrite = std::min(uiBitsLeftToWrite, m_uiBitsLeft);
        // calculate mask for bits
        uint8_t uiMask = (2 << (uiBitsToWrite - 1)) -1; 
        // calculate bits to written in next write if current byte has insufficient space
        uint8_t uiOverflowBits = (uiBitsLeftToWrite > m_uiBitsLeft)? uiBitsLeftToWrite - m_uiBitsLeft : 0;
        // align value to the right
        uint8_t uiOffsetByteValue = ((uiValueCopy >> uiOverflowBits) & uiMask);
        // now shift value to correct position within target byte
        uiOffsetByteValue = uiOffsetByteValue << (m_uiBitsLeft - uiBitsToWrite);
        // combine value in buffer with new written bits
        m_buffer[m_uiCurrentBytePos] = m_buffer[m_uiCurrentBytePos] | uiOffsetByteValue;
        
        uiBitsLeftToWrite -= uiBitsToWrite;
        m_uiBitsLeft -= uiBitsToWrite;
        if (m_uiBitsLeft == 0)
        {
          m_uiBitsLeft = 8;
          ++m_uiCurrentBytePos;
        }
        // subtract written bits from value 
        uiValueCopy = uiValueCopy - (uiOffsetByteValue << uiOverflowBits);
      }

#if 0
      //
      //
      uint8_t uiBitsToUse = std::min(uiBits, m_uiBitsLeft);
      uint8_t uiMask = (2 << (uiBits - 1)); 
      uint8_t uiMaskedValue = uiValue & uiMask;
      LOG_INFO(rLogger, LOG_FUNCTION, "Mask: %1% Masked Val: %2%", (int)uiMask, (int)uiMaskedValue);
      // current value at posi
      // check if we have enough bits for all value
      if (uiBits <= m_uiBitsLeft)
      {
        LOG_INFO(rLogger, LOG_FUNCTION, "Before: %1% After shift: %2%", (int)m_buffer[m_uiCurrentBytePos] , (int)(m_buffer[m_uiCurrentBytePos] << uiBits) );
     
        m_buffer[m_uiCurrentBytePos] = (m_buffer[m_uiCurrentBytePos] << uiBits) | uiMaskedValue;
        m_uiBitsLeft -= uiBits;
        LOG_INFO(rLogger, LOG_FUNCTION, "New: %1% Bits left: %2%", (int)m_buffer[m_uiCurrentBytePos] , m_uiBitsLeft);
        if (m_uiBitsLeft == 0)
        {
          m_uiBitsLeft = 8;
          ++m_uiCurrentBytePos;
        }
      }
      else
      {
        uint32_t uiRemainderBits = uiBits - m_uiBitsLeft;
        // fill up current byte
        m_buffer[m_uiCurrentBytePos]  = (m_buffer[m_uiCurrentBytePos]  << m_uiBitsLeft) | (uiMaskedValue >> uiRemainderBits);
        m_uiBitsLeft = 8;
        ++m_uiCurrentBytePos;
        // write remainder
        uiMask = (2 << uiRemainderBits) - 1;
        m_buffer[m_uiCurrentBytePos]  = uiMaskedValue & uiMask; // the new mask will get rid of the other bits
        m_uiBitsLeft = 8 - uiRemainderBits;
      }
#endif
    }
  }

  // this method can only be called on byte boundaries
  bool writeBytes(const uint8_t*& rSrc, uint32_t uiBytes)
  {
    if ((m_uiBitsLeft != 8) || // check byte boundary
        ((m_uiBufferSize - m_uiCurrentBytePos) < uiBytes) // check buffer size
       ) 
         return false;
    memcpy(&m_buffer[m_uiCurrentBytePos], rSrc, uiBytes);
    return true;
  }

  uint32_t bytesUsed() const 
  {
    return m_uiCurrentBytePos + (m_uiBitsLeft == 8 ? 0 : 1);
  }

  uint32_t totalBitsLeft() const
  {
    return  m_uiBitsLeft + 8 * (m_uiBufferSize - m_uiCurrentBytePos - 1);
  }

  Buffer str() const
  {
    // copy all bits to a buffer
    Buffer buffer;
    // first calculate size of buffer required
    uint32_t uiSize = bytesUsed(); 
    if (uiSize)
    {
      buffer.setData(new uint8_t[uiSize], uiSize);
      memcpy(&buffer[0], &m_buffer[0], uiSize); 
    }
    return buffer;
  }
private:
  void increaseBufferSize(uint32_t uiNewSize)
  {
      Buffer buffer = Buffer(new uint8_t[uiNewSize], uiNewSize);
      memset(&buffer[0], 0, uiNewSize);
      memcpy(&buffer[0], &m_buffer[0], m_uiBufferSize );
      m_buffer = buffer;
      m_uiBufferSize = uiNewSize;
  }
  void doWrite(uint32_t uiValue, uint32_t uiBits)
  {
    // calculate mask: (2^bits - 1) is a mask for uiBits number of bits
    uint8_t uiMask = (2 << (uiBits - 1)) - 1;
    uint8_t uiMaskedValue = uiValue & uiMask;
#if 0
    LOG_INFO(rLogger, LOG_FUNCTION, "Mask: %1% Masked Val: %2%", (int)uiMask, (int)uiMaskedValue);
#endif
    // current value at posi
    // check if we have enough bits for all value
    if (uiBits <= m_uiBitsLeft)
    {
#if 0
      LOG_INFO(rLogger, LOG_FUNCTION, "Before: %1% After shift: %2%", (int)m_buffer[m_uiCurrentBytePos] , (int)(m_buffer[m_uiCurrentBytePos] << uiBits) );
#endif

      m_buffer[m_uiCurrentBytePos] = (m_buffer[m_uiCurrentBytePos] << uiBits) | uiMaskedValue;
      m_uiBitsLeft -= uiBits;
#if 0
      LOG_INFO(rLogger, LOG_FUNCTION, "New: %1% Bits left: %2%", (int)m_buffer[m_uiCurrentBytePos] , m_uiBitsLeft);
#endif
      if (m_uiBitsLeft == 0)
      {
        m_uiBitsLeft = 8;
        ++m_uiCurrentBytePos;
      }
    }
    else
    {
      uint32_t uiRemainderBits = uiBits - m_uiBitsLeft;
      // fill up current byte
      m_buffer[m_uiCurrentBytePos]  = (m_buffer[m_uiCurrentBytePos]  << m_uiBitsLeft) & (uiMaskedValue >> uiRemainderBits);
      m_uiBitsLeft = 8;
      ++m_uiCurrentBytePos;
      // write remainder
      uiMask = (2 << uiRemainderBits) - 1;
      m_buffer[m_uiCurrentBytePos]  = uiMaskedValue & uiMask; // the new mask will get rid of the other bits
      m_uiBitsLeft = 8 - uiRemainderBits;
    }
  }

  uint32_t m_uiBufferSize;
  //buffer_t m_buffer;
  Buffer m_buffer;

  ///< Bits left in current byte
  uint32_t m_uiBitsLeft;

  ///< Current position in the buffer  
  uint32_t m_uiCurrentBytePos; 
};

// TODO: would this help to output pairs
//OBitStream& operator<<( OBitStream& stream, std::pair<uint32_t, uint32_t> )
//{
//
//}

