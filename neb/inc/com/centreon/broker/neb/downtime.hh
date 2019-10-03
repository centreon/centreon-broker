/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_NEB_DOWNTIME_HH
#define CCB_NEB_DOWNTIME_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class downtime downtime.hh "com/centreon/broker/neb/downtime.hh"
 *  @brief Represents a downtime inside Nagios.
 *
 *  A user may have the ability to define downtimes, which are
 *  time periods inside which some host or service shall not
 *  generate any notification. This can occur when a system
 *  administrator perform maintenance on a server for example.
 */
class downtime : public io::data {
 public:
  downtime();
  downtime(downtime const& other);
  ~downtime();
  downtime& operator=(downtime const& other);
  uint32_t type() const;
  bool operator==(downtime const& other) const;

  /**
   *  Get the type of this event.
   *
   *  @return  The event type.
   */
  static uint32_t static_type() {
    return (io::events::data_type<io::events::neb, neb::de_downtime>::value);
  }

  timestamp actual_end_time;
  timestamp actual_start_time;
  std::string author;
  std::string comment;
  timestamp deletion_time;
  short downtime_type;
  timestamp duration;
  timestamp end_time;
  timestamp entry_time;
  bool fixed;
  uint32_t host_id;
  uint32_t internal_id;
  uint32_t poller_id;
  uint32_t service_id;
  timestamp start_time;
  uint32_t triggered_by;
  bool was_cancelled;
  bool was_started;
  bool is_recurring;
  std::string recurring_timeperiod;
  short come_from;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(downtime const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_DOWNTIME_HH
