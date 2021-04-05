/*
** Copyright 2021 Centreon
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

#ifndef CCB_BBDO_STOP_HH
#define CCB_BBDO_STOP_HH

#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bbdo {
/**
 *  @class stop stop.hh "com/centreon/broker/bbdo/stop.hh"
 *  @brief An stop event sent between two bbdo's endpoint.
 *
 *  When it is time for engine to stop, it sends this event to its peers.
 *  When a peer receives it, it answers with an ack event. Engine can then
 *  acknowledge on its side events received by broker.
 */
class stop : public io::data {
 public:
  stop();
  ~stop() noexcept = default;
  stop(const stop&) = delete;
  stop& operator=(const stop&) = delete;

  /**
   *  Get the event type.
   *
   *  @return The event type.
   */
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bbdo, bbdo::de_stop>::value;
  }

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace bbdo

CCB_END()

#endif  // !CCB_BBDO_STOP_HH
