#pragma once
#include <cstdint>

/**
 * @brief The BitReader class duplicates code from the IBitStream for now.
 * This code does not need a Buffer and can parse the passed in pointer!
 * It does not take ownership of the data pointed to.
 */
class BitReader
{
public:
  BitReader(const uint8_t* pStream, uint32_t uiLength)
    :m_pBitStream(pStream),
      m_uiLength(uiLength),
      m_uiBitsRemaining(uiLength << 3),
      m_uiBitsInCurrentByte(8),
      m_uiCurrentBytePos(0)
  {

  }

  uint32_t getBitsRemaining() const { return m_uiBitsRemaining; }
  uint32_t getBytesRemaining() const { return (m_uiBitsRemaining >> 3); }

  bool read(uint32_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > m_uiBitsRemaining)
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_pBitStream[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  // This method can only read 8 bits at a time
  bool read(uint8_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > 8 || (uiBits > m_uiBitsRemaining))
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_pBitStream[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  // This method can only read 16 bits at a time
  bool read(uint16_t& uiValue, uint32_t uiBits)
  {
    if (uiBits > 16 || (uiBits > m_uiBitsRemaining))
    {
      return false;
    }

    uiValue = 0;
    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining > 0)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      uiValue <<= uiBitsToReadInCurrentByte;
      // bits to shift by
      uint32_t uiBitsToShiftBy = m_uiBitsInCurrentByte - uiBitsToReadInCurrentByte;
      uint8_t uiMask = (2 << (uiBitsToReadInCurrentByte - 1)) - 1;
      uiValue |= ((m_pBitStream[m_uiCurrentBytePos] >> uiBitsToShiftBy) & uiMask);

      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    // update total
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  // this method can only be called on byte boundaries
  bool readBytes(uint8_t*& rDestination, uint32_t uiBytes)
  {
    uint32_t uiBits = uiBytes << 3;
    if ((m_uiBitsInCurrentByte != 8) ||
        (uiBits > m_uiBitsRemaining)
       )
    {
      return false;
    }

    memcpy(rDestination, &m_pBitStream[m_uiCurrentBytePos], uiBytes);
    // update total
    m_uiBitsRemaining -= uiBits;
    m_uiCurrentBytePos += uiBytes;
    return true;
  }

  bool skipBits(uint32_t uiBits)
  {
    if (uiBits > m_uiBitsRemaining)
    {
      return false;
    }

    uint32_t uiBitsRemaining = uiBits;
    while (uiBitsRemaining)
    {
      uint32_t uiBitsToReadInCurrentByte = std::min(m_uiBitsInCurrentByte, uiBitsRemaining);
      // preserve old value
      m_uiBitsInCurrentByte -= uiBitsToReadInCurrentByte;
      if (m_uiBitsInCurrentByte == 0)
      {
        m_uiBitsInCurrentByte = 8;
        ++m_uiCurrentBytePos;
      }
      uiBitsRemaining -= uiBitsToReadInCurrentByte;
    }
    m_uiBitsRemaining -= uiBits;
    return true;
  }

  bool skipBytes(uint32_t uiBytes)
  {
    uint32_t uiBits = uiBytes << 3;
    if ((m_uiBitsInCurrentByte != 8) ||
      (uiBits > m_uiBitsRemaining)
      )
    {
      return false;
    }

    m_uiBitsRemaining -= uiBits;
    m_uiCurrentBytePos += uiBytes;
    return true;
  }

  uint8_t peekAtCurrentByte() const
  {
    return m_pBitStream[m_uiCurrentBytePos];
  }

private:
  const uint8_t* m_pBitStream;
  uint32_t m_uiLength;

  uint32_t m_uiBitsRemaining;
  uint32_t m_uiBitsInCurrentByte;
  uint32_t m_uiCurrentBytePos;
};
