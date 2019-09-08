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

#ifndef CC_CONCURRENCY_READ_WRITE_LOCK_POSIX_HH
#define CC_CONCURRENCY_READ_WRITE_LOCK_POSIX_HH

#include <pthread.h>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace concurrency {
/**
 *  @class read_write_lock read_write_lock_posix.hh
 * "com/centreon/concurrency/read_write_lock.hh"
 *  @brief Readers/writer lock.
 *
 *  Implementation of the readers/writer lock synchronization
 *  primitive.
 */
class read_write_lock {
 public:
  read_write_lock();
  ~read_write_lock() throw();
  void read_lock();
  bool read_lock(unsigned long timeout);
  bool read_trylock();
  void read_unlock();
  void write_lock();
  bool write_lock(unsigned long timeout);
  bool write_trylock();
  void write_unlock();

 private:
  read_write_lock(read_write_lock const& right);
  read_write_lock& operator=(read_write_lock const& right);

  pthread_rwlock_t _rwl;
};
}  // namespace concurrency

CC_END()

#endif  // !CC_CONCURRENCY_READ_WRITE_LOCK_POSIX_HH
