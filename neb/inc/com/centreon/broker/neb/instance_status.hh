/*
** Copyright 2009-2012,2015 Centreon
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

#ifndef CCB_EVENTS_INSTANCE_STATUS_HH
#define CCB_EVENTS_INSTANCE_STATUS_HH

#include <string>
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/status.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class instance_status instance_status.hh
 * "com/centreon/broker/neb/instance_status.hh"
 *  @brief Information about Nagios process.
 *
 *  instance_status holds information about a scheduling
 *  process, like whether it is running or not, in daemon mode
 *  or not, ...
 */
class instance_status : public status {
 public:
  instance_status();
  instance_status(instance_status const& other);
  ~instance_status();
  instance_status& operator=(instance_status const& other);
  uint32_t type() const;
  static uint32_t static_type();

  bool active_host_checks_enabled;
  bool active_service_checks_enabled;
  bool check_hosts_freshness;
  bool check_services_freshness;
  std::string global_host_event_handler;
  std::string global_service_event_handler;
  timestamp last_alive;
  timestamp last_command_check;
  bool obsess_over_hosts;
  bool obsess_over_services;
  bool passive_host_checks_enabled;
  bool passive_service_checks_enabled;
  uint32_t poller_id;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(instance_status const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_EVENTS_INSTANCE_STATUS_HH
