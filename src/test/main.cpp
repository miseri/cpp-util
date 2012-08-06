#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE ts_shared master_test_suite
#include <boost/test/unit_test.hpp>

#include <iostream>

#include <boost/asio/io_service.hpp>
#include <boost/chrono.hpp>

#include "Buffer.h"
#include "Clock.h"
#include "Conversion.h"
#include "IBitStream.h"
#include "OBitStream.h"
#include "RunningAverageQueue.h"

using namespace std;
using namespace boost::chrono;

BOOST_AUTO_TEST_CASE( tc1_test_buffer ) 
{
  Buffer buffer;
  OBitStream ob(12);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 0);
  BOOST_CHECK_EQUAL( 2, 2);
  // RTP version: 2 bits
  ob.write(2, 2);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 1);
  Buffer b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 1);
  // Padding: 1 bit
  ob.write(0, 1);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 1);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 1);
  // Extension: 1 bit
  ob.write(0, 1);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 1);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 1);
  // CC: 4 bits
  ob.write(0, 4);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 1);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 1);
  // Marker: 1 bit
  ob.write(0, 1);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 2);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 2);
  // Payload type: 7 bits
  ob.write(96, 7);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 2);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 2);
  // Sequence Number: 16 bits
  ob.write(257, 16);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 4);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 4);
  // Timestamp: 75BCD15
  ob.write(123456789, 32);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 8);
  b = ob.str();
  BOOST_CHECK_EQUAL( b.getSize(), 8);
}

BOOST_AUTO_TEST_CASE( tc1_test_bitstreams ) 
{
  const uint32_t RTP_VERSION    = 2;
  const uint32_t RTP_PADDING    = 0;
  const uint32_t RTP_EXTENSION  = 0;
  const uint32_t RTP_CC         = 0;
  const uint32_t RTP_MARKER     = 1;
  const uint32_t RTP_PAYLOAD    = 96;
  const uint32_t RTP_SN         = 257;
  const uint32_t RTP_TS         = 123456789;

  uint32_t RTP_VERSION2   = UINT_MAX;
  uint32_t RTP_PADDING2   = UINT_MAX;
  uint32_t RTP_EXTENSION2 = UINT_MAX;
  uint32_t RTP_CC2        = UINT_MAX;
  uint32_t RTP_MARKER2    = UINT_MAX;
  uint32_t RTP_PAYLOAD2   = UINT_MAX;
  uint32_t RTP_SN2        = UINT_MAX;
  uint32_t RTP_TS2        = UINT_MAX;

  OBitStream ob(12);
  // RTP version: 2 bits
  ob.write(RTP_VERSION, 2);
  // Padding: 1 bit
  ob.write(RTP_PADDING, 1);
  // Extension: 1 bit
  ob.write(RTP_EXTENSION, 1);
  // CC: 4 bits
  ob.write(RTP_CC, 4);
  // Marker: 1 bit
  ob.write(RTP_MARKER, 1);
  // Payload type: 7 bits
  ob.write(RTP_PAYLOAD, 7);
  // Sequence Number: 16 bits
  ob.write(RTP_SN, 16);
  // Timestamp: 75BCD15
  ob.write(RTP_TS, 32);
  Buffer buffer = ob.str();
  IBitStream ib(buffer);
  // RTP version: 2 bits
  ib.read(RTP_VERSION2, 2);
  // Padding: 1 bit
  ib.read(RTP_PADDING2, 1);
  // Extension: 1 bit
  ib.read(RTP_EXTENSION2, 1);
  // CC: 4 bits
  ib.read(RTP_CC2, 4);
  // Marker: 1 bit
  ib.read(RTP_MARKER2, 1);
  // Payload type: 7 bits
  ib.read(RTP_PAYLOAD2, 7);
  // Sequence Number: 16 bits
  ib.read(RTP_SN2, 16);
  // Timestamp: 75BCD15
  ib.read(RTP_TS2, 32);

  BOOST_CHECK_EQUAL( RTP_VERSION,   RTP_VERSION2);
  BOOST_CHECK_EQUAL( RTP_PADDING,   RTP_PADDING2);
  BOOST_CHECK_EQUAL( RTP_EXTENSION, RTP_EXTENSION2);
  BOOST_CHECK_EQUAL( RTP_CC,        RTP_CC2);
  BOOST_CHECK_EQUAL( RTP_MARKER,    RTP_MARKER2);
  BOOST_CHECK_EQUAL( RTP_PAYLOAD,   RTP_PAYLOAD2);
  BOOST_CHECK_EQUAL( RTP_SN,        RTP_SN2);
  BOOST_CHECK_EQUAL( RTP_TS,        RTP_TS2);
}

BOOST_AUTO_TEST_CASE( tc1_test_obitstream ) 
{
  Buffer buffer;
  OBitStream ob(12);

  BOOST_CHECK_EQUAL( ob.bytesUsed(), 0);
  // write 1 = '01'
  ob.write(1, 2);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 1);
  // write 1398101 = '010101 01010101 01010101' 
  ob.write(1398101, 22);
  BOOST_CHECK_EQUAL( ob.bytesUsed(), 3);
  Buffer b = ob.str();
  // should contain 85 85 85 = '01010101 01010101 01010101'
} 

BOOST_AUTO_TEST_CASE( tc1_test_string_to_bool_conversion )
{
  bool b = convert<bool>("1");
  BOOST_CHECK_EQUAL(b, true);
  b = convert<bool>("true");
  BOOST_CHECK_EQUAL(b, true);
  b = convert<bool>("True");
  BOOST_CHECK_EQUAL(b, true);

  b = convert<bool>("0");
  BOOST_CHECK_EQUAL(b, false);
  b = convert<bool>("False");
  BOOST_CHECK_EQUAL(b, false);
  b = convert<bool>("FALSE");
  BOOST_CHECK_EQUAL(b, false);
  b = convert<bool>("FaLSE");
  BOOST_CHECK_EQUAL(b, false);
  b = convert<bool>("");
  BOOST_CHECK_EQUAL(b, false);
}

BOOST_AUTO_TEST_CASE( tc_test_runningAverageQueue )
{
  uint32_t max = 20;
  RunningAverageQueue<uint32_t> queue1(max);
  RunningAverageQueue<uint32_t, double> queue2(max);

  uint32_t uiCount = 10;
  uint32_t uiTotal = 0;
  for (size_t i = 0; i < uiCount; ++i)
  {
    uiTotal += i;
    queue1.insert(i);
    queue2.insert(i);
  }

  BOOST_CHECK_EQUAL(queue1.getAverage(), uiTotal/uiCount);
  BOOST_CHECK_EQUAL(queue2.getAverage(), uiTotal/static_cast<double>(uiCount));
}


