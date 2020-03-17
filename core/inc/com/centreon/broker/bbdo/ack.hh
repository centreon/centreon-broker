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

#ifndef CCB_BBDO_ACK_HH
#define CCB_BBDO_ACK_HH

#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bbdo {
/**
 *  @class ack ack.hh "com/centreon/broker/bbdo/ack.hh"
 *  @brief An ack event sent between two bbdo's endpoint.
 *
 *  This is used for high-level event acknowledgement.
 */
class ack : public io::data {
 public:
  ack();
  ack(ack const& other);
  ~ack();
  ack& operator=(ack const& other);

  /**
   *  Get the event type.
   *
   *  @return The event type.
   */
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::bbdo, bbdo::de_ack>::value;
  }

  uint32_t acknowledged_events;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(ack const& right);
};
}  // namespace bbdo

CCB_END()

#endif  // !CCB_BBDO_VERSION_RESPONSE_HH
