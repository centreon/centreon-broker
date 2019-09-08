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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_TIMEPERIOD_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_COMPOSED_TIMEPERIOD_BUILDER_HH

#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/composed_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_builder.hh"
#include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class composed_timeperiod_builder composed_timeperiod_builder.hh
 * "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"
 *  @brief Composed timeperiod builder.
 *
 *  This class forward its method call to several other builders.
 */
class composed_timeperiod_builder
    : public composed_builder<timeperiod_builder> {
 public:
  composed_timeperiod_builder();
  virtual ~composed_timeperiod_builder() {}

  virtual void add_timeperiod(unsigned int id, time::timeperiod::ptr tperiod);
  virtual void add_timeperiod_exception(unsigned int timeperiod_id,
                                        std::string const& days,
                                        std::string const& timerange);
  virtual void add_timeperiod_exclude_relation(unsigned int timeperiod_id,
                                               unsigned int exclude_id);
  virtual void add_timeperiod_include_relation(unsigned int timeperiod_id,
                                               unsigned int include_id);
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_COMPOSED_TIMEPERIOD_BUILDER_HH
