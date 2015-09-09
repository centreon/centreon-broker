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

#include "com/centreon/broker/time/timezone_locker.hh"
#include "com/centreon/broker/time/timezone_manager.hh"

using namespace com::centreon::broker::time;

/**
 *  Constructor.
 *
 *  @param[in] tz  Timezone to set during object lifetime.
 */
timezone_locker::timezone_locker(char const* tz) {
  timezone_manager::instance().lock();
  timezone_manager::instance().push_timezone(tz);
}

/**
 *  Destructor.
 */
timezone_locker::~timezone_locker() {
  timezone_manager::instance().pop_timezone();
  timezone_manager::instance().unlock();
}
