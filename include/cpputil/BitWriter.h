#pragma once
#include <cassert>
#include <cstring>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_array.hpp>
#include "Buffer.h"
#include "IBitStream.h"

#define DEFAULT_BUFFER_SIZE 1024

/**
 * @brief The BitWriter class duplicates code from the BitWriter for now.
 * This code does not need a Buffer and can write to the passed in pointer!
 * It does not take ownership of the data pointed to.
 */
class BitWriter
{
public:

  BitWriter(uint8_t* pDestination, uint32_t uiLength)
    :m_uiBufferSize(uiLength),
    m_pDestination(pDestination),
    m_uiBitsLeft(8),
    m_uiCurrentBytePos(0)
  {
    memset(m_pDestination, 0, m_uiBufferSize);
  }

  /**
    * This method resets the write pointers inside the class
    * The allocated memory is not touched or modified.
    * This is useful to avoid memory allocation.
    */
  void reset()
  {
    m_uiBitsLeft = 8;
    m_uiCurrentBytePos = 0;
    memset(m_pDestination, 0, m_uiBufferSize);
  }

  bool write8Bits(uint8_t uiValue)
  {
    if (totalBitsLeft() < 8)
    {
      return false;
    }
    // check if we're writing on byte boundary
    if (m_uiBitsLeft == 8)
    {
      m_pDestination[m_uiCurrentBytePos++] = uiValue;
    }
    else
    {
      write(uiValue, 8);
    }
    return true;
  }

  bool write(uint32_t uiValue, uint32_t uiBits)
  {
    // check if enough memory has been allocated
    if ( totalBitsLeft() < uiBits )
    {
      return false;
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
#ifdef DEBUG_BitWriter
            DLOG(INFO) << 32;
#endif
            m_pDestination[m_uiCurrentBytePos++] = ((uiValue >> 24) & 0xff);
          }
          case 24:
          {
#ifdef DEBUG_BitWriter
            DLOG(INFO) << 24;
#endif
            m_pDestination[m_uiCurrentBytePos++] = ((uiValue >> 16) & 0xff);
          }
          case 16:
          {
#ifdef DEBUG_BitWriter
            DLOG(INFO) << 16;
#endif
            m_pDestination[m_uiCurrentBytePos++] = ((uiValue >> 8) & 0xff);
          }
          case 8:
          {
#ifdef DEBUG_BitWriter
            DLOG(INFO) << 8;
#endif
            m_pDestination[m_uiCurrentBytePos++] = uiValue;
            break;
          }
        }
      }
      else
      {
        //memcpy(&m_pDestinations[m_uiCurrentBufferPos][m_uiCurrentBytePos],  );
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
        m_pDestination[m_uiCurrentBytePos] = m_pDestination[m_uiCurrentBytePos] | uiOffsetByteValue;

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
    }
    return true;
  }

  // this method can only be called on byte boundaries
  bool writeBytes(const uint8_t*& rSrc, uint32_t uiBytes)
  {
    if ((m_uiBitsLeft != 8) || // check byte boundary
        ((m_uiBufferSize - m_uiCurrentBytePos) < uiBytes) // check buffer size
       )
         return false;
    memcpy(&m_pDestination[m_uiCurrentBytePos], rSrc, uiBytes);
    m_uiCurrentBytePos += uiBytes;
    return true;
  }


  // this method writes all bytes remaining in the IBitStream to the output stream
  // TODO: make this method handle non-byte boundary data
  bool write(IBitStream& in)
  {
      if (m_uiBitsLeft != 8) return false;
      // get remaining bytes
      if (in.getBitsRemaining() % 8 != 0) return false;

      // this code only works if all the pointers are byte aligned!!!
      if (m_uiCurrentBytePos >= m_uiBufferSize)
      {
          LOG(WARNING) << "WARN: Byte pos: " << m_uiCurrentBytePos << " Size: " << m_uiBufferSize;
      }
      assert (m_uiCurrentBytePos <= m_uiBufferSize);
      uint32_t uiBytesLeft = m_uiBufferSize - m_uiCurrentBytePos;

      uint32_t uiBytesToCopy = in.getBytesRemaining();
      if (uiBytesToCopy > uiBytesLeft)
      {
        return false;
      }

      uint8_t* pDestination = m_pDestination + m_uiCurrentBytePos;
      bool bRes = in.readBytes(pDestination, uiBytesToCopy);
      assert (bRes);
      m_uiCurrentBytePos += uiBytesToCopy;
      return bRes;
  }

  // this method writes all bytes remaining in the IBitStream to the output stream
  // TODO: make this method handle non-byte boundary data
  bool write(IBitStream& in, uint32_t uiBytesToCopy)
  {
#if 0
      VLOG(5) << "bits left: " << m_uiBitsLeft << " bits remaining: " << in.m_uiBitsRemaining << " Bytes: " << in.getBytesRemaining() << " To copy: " << uiBytesToCopy;
#endif
      if (m_uiBitsLeft != 8) return false;
      // get remaining bytes
      if (in.getBitsRemaining() % 8 != 0) return false;
      if (in.getBytesRemaining() < uiBytesToCopy) return false;

      // this code only works if all the pointers are byte aligned!!!
      if (m_uiCurrentBytePos >= m_uiBufferSize)
      {
          LOG(WARNING) << "WARN: Byte pos: " << m_uiCurrentBytePos << " Size: " << m_uiBufferSize;
      }
      assert (m_uiCurrentBytePos <= m_uiBufferSize);
      uint32_t uiBytesLeft = m_uiBufferSize - m_uiCurrentBytePos;

      if (uiBytesToCopy > uiBytesLeft)
      {
        return false;
      }
      uint8_t* pDestination = m_pDestination+ m_uiCurrentBytePos;
      bool bRes = in.readBytes(pDestination, uiBytesToCopy);
      assert (bRes);
      m_uiCurrentBytePos += uiBytesToCopy;
      return bRes;
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
      memcpy(&buffer[0], &m_pDestination[0], uiSize);
    }
    return buffer;
  }

  Buffer data() const
  {
    return str();
  }

private:
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
      LOG_INFO(rLogger, LOG_FUNCTION, "Before: %1% After shift: %2%", (int)m_pDestination[m_uiCurrentBytePos] , (int)(m_pDestination[m_uiCurrentBytePos] << uiBits) );
#endif

      m_pDestination[m_uiCurrentBytePos] = (m_pDestination[m_uiCurrentBytePos] << uiBits) | uiMaskedValue;
      m_uiBitsLeft -= uiBits;
#if 0
      LOG_INFO(rLogger, LOG_FUNCTION, "New: %1% Bits left: %2%", (int)m_pDestination[m_uiCurrentBytePos] , m_uiBitsLeft);
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
      m_pDestination[m_uiCurrentBytePos]  = (m_pDestination[m_uiCurrentBytePos]  << m_uiBitsLeft) & (uiMaskedValue >> uiRemainderBits);
      m_uiBitsLeft = 8;
      ++m_uiCurrentBytePos;
      // write remainder
      uiMask = (2 << uiRemainderBits) - 1;
      m_pDestination[m_uiCurrentBytePos]  = uiMaskedValue & uiMask; // the new mask will get rid of the other bits
      m_uiBitsLeft = 8 - uiRemainderBits;
    }
  }

  uint32_t m_uiBufferSize;
  uint8_t* m_pDestination;
  ///< Bits left in current byte
  uint32_t m_uiBitsLeft;
  ///< Current position in the buffer
  uint32_t m_uiCurrentBytePos;
};


