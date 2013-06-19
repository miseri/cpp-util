#pragma once

#include <boost/function.hpp>

/**
 * The startEventLoop function takes care of ending console 
 * applications under windows and linux and is based on Chris
 * Kohlhoff's examples in the boost asio library.
 */

typedef boost::function0<void> ctrl_function_t;
// global stop function
ctrl_function_t console_ctrl_start_function;
ctrl_function_t console_ctrl_stop_function;

#ifdef _WIN32
#include <Windows.h>
#include <WinSock2.h>
#include <boost/bind.hpp>


BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
  switch (ctrl_type)
  {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_CLOSE_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    console_ctrl_stop_function();
    return TRUE;
  default:
    return FALSE;
  }
}
#else
#include <pthread.h>
#include <signal.h>
#include <boost/thread.hpp>
#endif

static void startEventLoop(ctrl_function_t start_function, ctrl_function_t stop_function)
{
  console_ctrl_start_function = start_function;
  console_ctrl_stop_function = stop_function;


#ifdef _WIN32
  SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
  console_ctrl_start_function(); // blocking call
  console_ctrl_stop_function = NULL;
#else
  // Block all signals for background thread
  sigset_t newMask;
  sigfillset(&newMask);
  sigset_t oldMask;
  pthread_sigmask(SIG_BLOCK, &newMask, &oldMask);

  // run server in background thread
  boost::thread t(start_function);

  // restore previous signals
  pthread_sigmask(SIG_SETMASK, &oldMask, 0);
  // wait for signal indicating time to shut down
  sigset_t waitMask;
  sigemptyset(&waitMask);
  sigaddset(&waitMask, SIGINT);
  sigaddset(&waitMask, SIGQUIT);
  sigaddset(&waitMask, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &waitMask, 0);
  int sig = 0;

  VLOG(2) << "Waiting for signal";
  sigwait(&waitMask, &sig);

  VLOG(2) << "Signal received: calling stop function";
  stop_function();

  VLOG(2) << "Joining thread";
  // wait for thread to finish
  t.join();
  VLOG(2) << "Join complete";
#endif
}

