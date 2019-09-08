/*
** Copyright 2012-2013 Centreon
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

#ifndef CC_CONCURRENCY_MUTEX_WIN32_HH
#define CC_CONCURRENCY_MUTEX_WIN32_HH

#include <windows.h>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
/**
 *  @class mutex mutex_win32.hh "com/centreon/concurrency/mutex.hh"
 *  @brief Implements a mutex.
 *
 *  Win32-base implementation of a mutex (intra-process
 *  synchronisation). On Windows the mutex class is a critical
 *  section and should not be confused with a Win32 mutex which is
 *  used for inter-process synchronisation.
 */
class mutex {
  friend class condvar;

 public:
  mutex();
  ~mutex() throw();
  void lock();
  bool trylock();
  void unlock();

 private:
  mutex(mutex const& right);
  mutex& operator=(mutex const& right);

  CRITICAL_SECTION _csection;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_MUTEX_WIN32_HH
