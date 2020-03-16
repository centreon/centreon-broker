/*
** Copyright 2017 Centreon
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

#ifndef CCB_GENERATOR_DUMMY_HH
#define CCB_GENERATOR_DUMMY_HH

#include "com/centreon/broker/generator/internal.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace generator {
/**
 *  @class dummy dummy.hh "com/centreon/broker/generator/dummy.hh"
 *  @brief Dummy generated event.
 *
 *  This is the base event transmitted by the generator module. It
 *  contains a monotonic incremental integer used to verify that the
 *  event order is not broken.
 */
class dummy : public io::data {
 public:
  dummy(uint32_t number = 0);
  dummy(dummy const& other);
  ~dummy();
  dummy& operator=(dummy const& other);

  /**
   *  Get the type of this event.
   *
   *  @return  The event type.
   */
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::generator,
                                 generator::de_dummy>::value;
  }

  uint32_t number;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace generator

CCB_END()

#endif  // !CCB_GENERATOR_DUMMY_HH
