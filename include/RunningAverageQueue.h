#pragma once

#include <algorithm>
#include <deque>
#include <numeric>
#include <stdexcept>

/**
  * Utility class to calculate the running average of values
  */

template <typename T, typename R = T>
class RunningAverageQueue
{
public:
  RunningAverageQueue(std::size_t max_size)
    :m_max_size(max_size),
    m_average(0) // is this ok as the default value?
  {
    if (m_max_size == 0) throw std::invalid_argument("Bad size");
  }

  R getAverage() const { return m_average; }

  void insert(const T t)
  {
    m_queue.push_back(t);
    if (m_queue.size() > m_max_size)
      m_queue.pop_front();

    m_average = std::accumulate(m_queue.begin(), m_queue.end(), 0)/static_cast<R>(m_queue.size());
  }

private:

  std::size_t m_max_size;
  std::deque<T> m_queue;
  R m_average;
};
