/*
** Copyright 2014 Centreon
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

#ifndef CCB_CORE_TIME_TIMEZONE_LOCKER_HH
#define CCB_CORE_TIME_TIMEZONE_LOCKER_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class timezone_locker timezone_locker.hh
 * "com/centreon/broker/time/timezone_locker.hh"
 *  @brief Handle timezone changes, even in case of exception.
 *
 *  This class works on a timezone_manager to push a new timezone at
 *  construction and pop it when destructed.
 */
class timezone_locker {
 public:
  timezone_locker(char const* tz);
  ~timezone_locker();

 private:
  timezone_locker(timezone_locker const& other);
  timezone_locker& operator=(timezone_locker const& other);
};
}  // namespace time

CCB_END()

#endif  // !CCB_CORE_TIME_TIMEZONE_LOCKER_HH
