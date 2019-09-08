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

#ifndef CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_LINKER_HH
#define CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_LINKER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/timeperiod_builder.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class timeperiod_linker timeperiod_linker.hh
 * "com/centreon/broker/notification/builders/timeperiod_linker.hh"
 *  @brief Timeperiod linker.
 *
 *  Links the timeperiod together.
 */
class timeperiod_linker : public timeperiod_builder {
 public:
  timeperiod_linker();

  void add_timeperiod(unsigned int id, time::timeperiod::ptr con);

  void add_timeperiod_exception(unsigned int timeperiod_id,
                                std::string const& days,
                                std::string const& timerange);
  void add_timeperiod_exclude_relation(unsigned int timeperiod_id,
                                       unsigned int exclude_id);
  void add_timeperiod_include_relation(unsigned int timeperiod_id,
                                       unsigned int include_id);

 private:
  QHash<unsigned int, time::timeperiod::ptr> _table;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_LINKER_HH
