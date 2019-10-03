/*
** Copyright 2009-2013,2015 Centreon
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

#ifndef CCB_NEB_HOST_SERVICE_STATUS_HH
#define CCB_NEB_HOST_SERVICE_STATUS_HH

#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/status.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class host_service_status host_service_status.hh
 * "com/centreon/broker/neb/host_service_status.hh"
 *  @brief host_status and service_status shared data.
 *
 *  This class represents what is shared between a host status
 *  event and a service status event.
 *
 *  @see host_status
 *  @see service_status
 */
class host_service_status : public status {
 public:
  host_service_status();
  host_service_status(host_service_status const& hss);
  virtual ~host_service_status();
  host_service_status& operator=(host_service_status const& hss);

  bool acknowledged;
  short acknowledgement_type;
  bool active_checks_enabled;
  std::string check_command;
  double check_interval;
  std::string check_period;
  short check_type;
  short current_check_attempt;
  short current_state;
  short downtime_depth;
  bool enabled;
  std::string event_handler;
  double execution_time;
  bool has_been_checked;
  uint32_t host_id;
  bool is_flapping;
  timestamp last_check;
  short last_hard_state;
  timestamp last_hard_state_change;
  timestamp last_notification;
  timestamp last_state_change;
  timestamp last_update;
  double latency;
  short max_check_attempts;
  timestamp next_check;
  timestamp next_notification;
  bool no_more_notifications;
  short notification_number;
  bool obsess_over;
  std::string output;
  bool passive_checks_enabled;
  double percent_state_change;
  std::string perf_data;
  double retry_interval;
  bool should_be_scheduled;
  short state_type;

 private:
  void _internal_copy(host_service_status const& hss);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_HOST_SERVICE_STATUS_HH
