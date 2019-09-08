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

#ifndef CC_CONCURRENCY_WRITE_LOCKER_HH
#define CC_CONCURRENCY_WRITE_LOCKER_HH

#include "com/centreon/concurrency/read_write_lock.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
/**
 *  @class write_locker write_locker.hh
 * "com/centreon/concurrency/write_locker.hh"
 *  @brief Handle write locking of a readers-writer lock.
 *
 *  Lock RWL upon creation and unlock it upon destruction.
 */
class write_locker {
 public:
  /**
   *  Constructor.
   */
  write_locker(read_write_lock* rwl) : _locked(false), _rwl(rwl) { relock(); }

  /**
   *  Destructor.
   */
  ~write_locker() throw() {
    try {
      if (_locked)
        unlock();
    } catch (...) {
    }
  }

  /**
   *  Relock.
   */
  void relock() {
    _rwl->write_lock();
    _locked = true;
    return;
  }

  /**
   *  Unlock.
   */
  void unlock() {
    _rwl->write_unlock();
    _locked = false;
    return;
  }

 private:
  write_locker(write_locker const& right);
  write_locker& operator=(write_locker const& right);

  bool _locked;
  read_write_lock* _rwl;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_WRITE_LOCKER_HH
