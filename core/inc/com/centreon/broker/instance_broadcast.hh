/*
** Copyright 2013 Centreon
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

#ifndef CCB_INSTANCE_BROADCAST_HH
#define CCB_INSTANCE_BROADCAST_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class instance_broadcast instance_broadcast.hh
 * "com/centreon/broker/instance_broadcast.hh"
 *  @brief Broadcast the instance information at startup.
 */
class instance_broadcast : public io::data {
 public:
  instance_broadcast();
  instance_broadcast(instance_broadcast const& other);
  ~instance_broadcast();
  instance_broadcast& operator=(instance_broadcast const& other);
  uint32_t type() const;

  /**
   *  Get the event type.
   *
   *  @return The event type.
   */
  static uint32_t static_type() {
    return (io::events::data_type<io::events::internal,
                                  io::events::de_instance_broadcast>::value);
  }

  uint32_t broker_id;
  std::string broker_name;
  bool enabled;
  uint32_t poller_id;
  std::string poller_name;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

  static void load();

 private:
  void _internal_copy(instance_broadcast const& other);
};

CCB_END()

#endif  // !CCB_INSTANCE_BROADCAST_HH
