/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CC_CONCURRENCY_THREAD_POSIX_HH
#define CC_CONCURRENCY_THREAD_POSIX_HH

#include <pthread.h>
#include "com/centreon/concurrency/mutex_posix.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
typedef pthread_t thread_id;

/**
 *  @class thread thread_posix.hh "com/centreon/concurrency/thread.hh"
 *  @brief POSIX thread wrapper.
 *
 *  Wrap POSIX thread library (pthreads) in a nice and easy to use
 *  class.
 */
class thread {
 public:
  thread();
  virtual ~thread() throw();
  void exec();
  static thread_id get_current_id() throw();
  static void msleep(unsigned long msecs);
  static void nsleep(unsigned long nsecs);
  static void sleep(unsigned long secs);
  static void usleep(unsigned long usecs);
  void wait();
  bool wait(unsigned long timeout);
  static void yield() throw();

 protected:
  virtual void _run() = 0;

 private:
  thread(thread const& right);
  thread& operator=(thread const& right);
  static void* _execute(void* data);

  bool _initialized;
  mutable mutex _mtx;
  pthread_t _th;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_THREAD_POSIX_HH
