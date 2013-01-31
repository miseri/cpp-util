#pragma once
#include <boost/chrono.hpp>

/** 
 * Timer based on boost chrono
 */
template< class Clock >
class Timer
{
  typename Clock::time_point start;
public:
  Timer() : start( Clock::now() ) {}
  typename Clock::duration elapsed() const
  {
    return Clock::now() - start;
  }
  double seconds() const
  {
    return elapsed().count() * ((double)Clock::period::num/Clock::period::den);
  }
};

typedef Timer<boost::chrono::system_clock> SystemClock_t;
typedef Timer<boost::chrono::steady_clock> SteadyClock_t;
typedef Timer<boost::chrono::high_resolution_clock> HighResolutionClock_t;
