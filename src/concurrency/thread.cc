/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <memory>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "concurrency/thread.hh"
#include "concurrency/thread_listener.hh"
#include "exceptions/retval.hh"

using namespace concurrency;

/**************************************
*                                     *
*             Local Type              *
*                                     *
**************************************/

struct thread_helper_arg {
  thread*          t;
  thread_listener* tl;
};

/**************************************
*                                     *
*           Static Function           *
*                                     *
**************************************/

/**
 *  Entry point of all threads.
 */
static void* thread_helper(void* arg) {
  std::auto_ptr<thread_helper_arg> harg(
    static_cast<thread_helper_arg*>(arg));

  // Thread is starting.
  try {
    if (harg->tl)
      harg->tl->on_create(harg->t);
    harg->t->operator()();
  }
  catch (...) {
    // Exception caught. Thread execution failed.
    if (harg->tl)
      try {
        harg->tl->on_failure(harg->t);
      }
      catch (...) {}
    }
  // Thread will exit soon.
  if (harg->tl)
    try {
      harg->tl->on_exit(harg->t);
    }
    catch (...) {}

  return (NULL);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  As thread is not copyable, any attempt to use the copy constructor
 *  will result in a call to abort().
 *
 *  @param[in] t Unused.
 */
thread::thread(thread const& t) {
  (void)t;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  As thread is not copyable, any attempt to use this assignment
 *  operator will result in a call to abort().
 *
 *  @param[in] t Unused.
 *
 *  @return This object.
 */
thread& thread::operator=(thread const& t) {
  (void)t;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
thread::thread() : _joinable(false), _should_exit(true) {}

/**
 *  @brief Destructor.
 *
 *  If the thread is still running, it's completion will be awaited
 *  (join()'d) if it has not been detached.
 */
thread::~thread() {
  // If thread is running and is not detached.
  if (_joinable)
    try {
      // Wait for thread execution completion.
      join();
    }
    catch (...) {
      try {
        // If we couldn't reach the thread, try to cancel its execution.
        cancel();
        join();
      }
      catch (...) {}
    }
}

/**
 *  @brief Cancel thread execution.
 *
 *  This method should only be used on critical situations. No thread
 *  should be terminated this way. It should instead be detached or
 *  joined to ensure its proper termination. No exception will be thrown
 *  if either the thread is running or not.
 */
void thread::cancel() {
  pthread_cancel(_thread);
  return ;
}

/**
 *  @brief Detach the current thread.
 *
 *  Detaching a thread means that it can automatically be cleaned up by
 *  the operating system upon its termination. However, it won't be
 *  possible to join() it anymore. detach() shall only be called once
 *  per thread creation.
 */
void thread::detach() {
  if (_joinable) {
    int ret(pthread_detach(_thread));
    if (ret)
      throw (exceptions::retval(ret) << "could not detach thread: "
                                     << strerror(ret));
    _joinable = false;
  }
  else
    throw (exceptions::basic() << "thread has already been detached");
  return ;
}

/**
 *  Request thread to exit ASAP.
 */
void thread::exit() {
  _should_exit = true;
  return ;
}

/**
 *  @brief Waits for thread completion.
 *
 *  Waits for the current thread to terminate. The thread shall not have
 *  been detached.
 */
void thread::join() {
  if (_joinable) {
    void* ptr;
    int ret(pthread_join(_thread, &ptr));
    if (ret)
      throw (exceptions::retval(ret) << "could not join thread: "
                                     << strerror(ret));
    _joinable = false;
  }
  else
    throw (exceptions::basic() << "tried to join invalid or detached thread");
  return ;
}

/**
 *  @brief Run the thread.
 *
 *  The thread will be run using the operator()() method as overload.
 *
 *  @param[in] tl Optionnal thread listener.
 */
void thread::run(thread_listener* tl) {
  // Thread has not already been run.
  if (!_joinable) {
    std::auto_ptr<thread_helper_arg> arg(new thread_helper_arg);
    int ret;

    // Set the thread helper argument.
    arg->t = this;
    arg->tl = tl;
    _listener = tl;

    // Run the thread using an helper static method.
    _joinable = true;
    _should_exit = false;
    ret = pthread_create(&_thread, NULL, &thread_helper, arg.get());
    if (ret) {
      _joinable = false;
      _should_exit = true;
      throw (exceptions::retval(ret) << "could not create thread: "
                                     << strerror(ret));
    }
    else
      arg.release();
  }
  else
    throw (exceptions::basic() << "thread is already running " \
                                  "and has not been detached");

  return ;
}
