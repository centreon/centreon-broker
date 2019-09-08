/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_CORE_TIME_TIME_INFO_HH
#define CCB_CORE_TIME_TIME_INFO_HH

#include <ctime>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class time_info time_info.hh "com/centreon/broker/time/time_info.hh"
 *  @brief  Internal struct time information.
 */
struct time_info {
  time_t midnight;
  time_t preferred_time;
  tm preftime;
};
}  // namespace time

CCB_END()

#endif  // !CCB_CORE_TIME_TIME_INFO_HH
