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

#ifndef CCB_NEB_EVENT_HANDLER_HH
#define CCB_NEB_EVENT_HANDLER_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class event_handler event_handler.hh
 * "com/centreon/broker/neb/event_handler.hh"
 *  @brief Represents an event handler inside the scheduling engine.
 *
 *  Event handlers, as their name suggests, are executed upon
 *  the detection of some events by the scheduling engine.
 */
class event_handler : public io::data {
 public:
  event_handler();
  event_handler(event_handler const& other);
  ~event_handler();
  event_handler& operator=(event_handler const& other);
  unsigned int type() const;
  static unsigned int static_type();

  std::string command_args;
  std::string command_line;
  bool early_timeout;
  timestamp end_time;
  double execution_time;
  short handler_type;
  unsigned int host_id;
  std::string output;
  short return_code;
  unsigned int service_id;
  timestamp start_time;
  short state;
  short state_type;
  short timeout;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(event_handler const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_EVENT_HANDLER_HH
