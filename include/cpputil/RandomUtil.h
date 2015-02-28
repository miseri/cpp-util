#pragma once
#include <ctime>
#include <boost/cstdint.hpp>  
#include <boost/noncopyable.hpp>
#include <boost/random.hpp>

class RandomUtil : private boost::noncopyable
{
public:

  static RandomUtil& getInstance()
  {
    static RandomUtil randUtil;
    return randUtil;
  }

  uint16_t rand16()
  {
    uint16_t uiRandom = m_rng() % UINT_MAX;
    return uiRandom;
  }


  uint32_t rand32()
  {
    uint32_t uiRandom = m_rng();
    return uiRandom;
  }

private:

  RandomUtil()
  {
    m_rng.seed(static_cast<unsigned>(std::time(0)));
  }

  boost::mt19937 m_rng;
};


