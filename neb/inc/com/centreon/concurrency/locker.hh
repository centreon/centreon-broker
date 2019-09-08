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

#ifndef CC_CONCURRENCY_LOCKER_HH
#define CC_CONCURRENCY_LOCKER_HH

#include "com/centreon/concurrency/mutex.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
/**
 *  @class locker locker.hh "com/centreon/concurrency/locker.hh"
 *  @brief Provide a simple way to lock ans un lock mutex.
 *
 *  Allow simple method to lock and unlock mutex.
 */
class locker {
 public:
  /**
   *  Default constructor.
   *
   *  @param[in] m  The mutex to lock.
   */
  locker(mutex* m = NULL) : _is_lock(false), _m(m) {
    if (_m)
      relock();
  }

  /**
   *  Default destructor.
   */
  ~locker() throw() {
    if (_is_lock)
      unlock();
  }

  /**
   *  Get the mutex.
   *
   *  @return The internal mutex.
   */
  mutex* get_mutex() const throw() { return (_m); }

  /**
   *  Lock the internal mutex.
   */
  void relock() {
    if (_m) {
      _is_lock = true;
      _m->lock();
    }
  }

  /**
   *  Unlock the internal mutex.
   */
  void unlock() {
    if (_m) {
      _m->unlock();
      _is_lock = false;
    }
  }

 private:
  locker(locker const& right);
  locker& operator=(locker const& right);

  bool _is_lock;
  mutex* _m;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_LOCKER_HH
