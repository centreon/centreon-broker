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

#ifndef CC_CONCURRENCY_CONDVAR_POSIX_HH
#define CC_CONCURRENCY_CONDVAR_POSIX_HH

#include <pthread.h>
#include <climits>
#include "com/centreon/concurrency/mutex_posix.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
/**
 *  @class condvar condvar_posix.hh "com/centreon/concurrency/condvar.hh"
 *  @brief Allow simple threads synchronization.
 *
 *  Provide condition variable for synchronization between threads.
 */
class condvar {
 public:
  condvar();
  ~condvar() throw();
  void wait(mutex* mtx);
  bool wait(mutex* mtx, unsigned long timeout);
  void wake_all();
  void wake_one();

 private:
  condvar(condvar const& right);
  condvar& operator=(condvar const& right);

  pthread_cond_t _cnd;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_CONDVAR_POSIX_HH
