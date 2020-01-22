/*
** Copyright 2011-2012 Centreon
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

#ifndef CCB_PROCESSING_STAT_VISITABLE_HH
#define CCB_PROCESSING_STAT_VISITABLE_HH

#include <atomic>
#include <json11.hpp>
#include <mutex>
#include <string>
#include <unordered_set>
#include "com/centreon/broker/misc/processing_speed_computer.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace processing {
/**
 *  @class stat_visitable stat_visitable.hh
 * "com/centreon/broker/processing/stat_visitable.hh"
 *  @brief Represent a processing thread that is visitable.
 */
class stat_visitable {
  std::string _last_error;
  timestamp _last_connection_attempt;
  timestamp _last_connection_success;
  misc::processing_speed_computer _event_processing_speed;
  std::atomic<char const*> _state;
  std::atomic<uint32_t> _queued_events;

 protected:
  std::string _name;
  mutable std::mutex _stat_mutex;

  //virtual uint32_t _get_queued_events() = 0;
  virtual std::string const& _get_read_filters() const = 0;
  virtual std::string const& _get_write_filters() const = 0;
  virtual void _forward_statistic(json11::Json::object& tree);

 public:
  stat_visitable(std::string const& name = std::string());
  virtual ~stat_visitable() = default;
  stat_visitable(stat_visitable const& other) = delete;
  stat_visitable& operator=(stat_visitable const& other) = delete;

  std::string const& get_name() const;
  void set_last_error(std::string const& last_error);
  void set_state(char const* state);
  void set_queued_events(uint32_t);
  virtual void stats(json11::Json::object& tree);
  void set_last_connection_attempt(timestamp last_connection_attempt);
  void set_last_connection_success(timestamp last_connection_success);
  void tick(uint32_t events = 1);
};
}  // namespace processing

CCB_END()

#endif  // !CCB_PROCESSING_STAT_VISITABLE_HH
