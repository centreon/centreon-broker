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

#ifndef CCB_NEB_LOG_ENTRY_HH
#define CCB_NEB_LOG_ENTRY_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class log_entry log_entry.hh "com/centreon/broker/neb/log_entry.hh"
 *  @brief Log message.
 *
 *  From time to time, the scheduling engine generates a log
 *  message. These messages can be useful, especially when
 *  investigating problems. This class holds all information
 *  related to a log entry.
 */
class log_entry : public io::data {
 public:
  typedef enum {
    service_alert = 0,
    host_alert = 1,
    service_notification = 2,
    host_notification = 3,
    warning = 4,
    other = 5,
    service_initial_state = 8,
    host_initial_state = 9,
    service_acknowledge_problem = 10,
    host_acknowledge_problem = 11,
    service_event_handler = 12,
    host_event_handler = 13,
    global_service_event_handler = 14,
    global_host_event_handler = 15,
  } log_msg_type;

  log_entry();
  log_entry(log_entry const& other);
  ~log_entry();
  log_entry& operator=(const log_entry& other);
  uint32_t type() const;
  static uint32_t static_type();

  timestamp c_time;
  uint32_t host_id;
  std::string host_name;
  timestamp issue_start_time;
  short log_type;
  short msg_type;
  std::string notification_cmd;
  std::string notification_contact;
  std::string output;
  std::string poller_name;
  int retry;
  std::string service_description;
  uint32_t service_id;
  short status;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(log_entry const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_LOG_ENTRY_HH
