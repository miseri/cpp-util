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

  typename std::deque<T>::iterator begin()
  {
    return m_queue.begin();
  }

  typename std::deque<T>::iterator end()
  {
    return m_queue.end();
  }

  void clear() { m_queue.clear(); }
  bool empty() const { return m_queue.empty(); }
  std::size_t size() const { return m_queue.size(); }

  R getAverage() const { return m_average; }

  R getStandardDeviation() const { return m_standardDeviation; }

  void insert(const T t)
  {
    m_queue.push_back(t);
    if (m_queue.size() > m_max_size)
      m_queue.pop_front();

    // calc mean
    m_average = std::accumulate(m_queue.begin(), m_queue.end(), 0)/static_cast<R>(m_queue.size());

    // calc std dev
    // calc difference from mean
    std::vector<R> diff(m_queue.size());
    std::transform(m_queue.begin(), m_queue.end(), diff.begin(),
                     std::bind2nd(std::minus<R>(), m_average));

    R sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    m_standardDeviation = std::sqrt(sq_sum / m_queue.size());
  }

private:

  std::size_t m_max_size;
  std::deque<T> m_queue;
  R m_average;
  R m_standardDeviation;
};
