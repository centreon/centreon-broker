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

#ifndef CC_CONCURRENCY_CONDVAR_WIN32_HH
#define CC_CONCURRENCY_CONDVAR_WIN32_HH

#include <windows.h>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
// Forward declaration.
class mutex;

/**
 *  @class condvar condvar_win32.hh "com/centreon/concurrency/condvar.hh"
 *  @brief Simple thread synchronization.
 *
 *  Provide condition variable for synchronization between threads.
 */
class condvar {
 public:
  condvar();
  ~condvar() throw();
  void wait(mutex* mutx);
  bool wait(mutex* mutx, unsigned long timeout);
  void wake_all();
  void wake_one();

 private:
  condvar(condvar const& cv);
  condvar& operator=(condvar const& cv);
  bool _wait(mutex* mutx, DWORD timeout);

  CONDITION_VARIABLE _cond;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_CONDVAR_WIN32_HH
