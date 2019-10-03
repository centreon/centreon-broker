/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_CORRELATION_ENGINE_STATE_HH
#define CCB_CORRELATION_ENGINE_STATE_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace correlation {
/**
 *  @class engine_state engine_state.hh
 * "com/centreon/broker/correlation/engine_state.hh"
 *  @brief State of the correlation engine.
 *
 *  This class represent the state of the correlation engine.
 */
class engine_state : public io::data {
 public:
  engine_state();
  engine_state(engine_state const& es);
  virtual ~engine_state();
  engine_state& operator=(engine_state const& es);
  uint32_t type() const;
  static uint32_t static_type();

  uint32_t poller_id;
  bool started;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace correlation

CCB_END()

#endif  // !CCB_CORRELATION_STATE_HH
