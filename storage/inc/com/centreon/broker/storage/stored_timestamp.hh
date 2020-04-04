/*
** Copyright 2014 Centreon
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

#ifndef CCB_SQL_STORED_TIMESTAMP_HH
#define CCB_SQL_STORED_TIMESTAMP_HH

#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace storage {
/**
 *  @class stored_timestamp stored_timestamp.hh
 * "com/centreon/broker/sql/stored_timestamp.hh"
 *  @brief Store a timestamp associated to an instance.
 *
 *  The instance can be in a responsive or unresponsive state, and its id is
 * stored.
 */
class stored_timestamp {
 public:
  enum state_type { responsive, unresponsive };

 private:
  uint32_t _instance_id;
  timestamp _ts;
  state_type _state;

 public:
  stored_timestamp() noexcept;
  stored_timestamp(uint32_t id, state_type s) noexcept;
  stored_timestamp(stored_timestamp const& right) noexcept;
  ~stored_timestamp() noexcept = default;

  stored_timestamp& operator=(stored_timestamp const&) = default;

  uint32_t get_id() const noexcept;
  state_type get_state() const noexcept;
  void set_state(state_type state) noexcept;
  void update_timestamp() noexcept;
  timestamp get_timestamp() const noexcept;
  void set_timestamp(timestamp ts) noexcept;
  bool timestamp_outdated(uint32_t timeout) const noexcept;
};
}  // namespace storage

CCB_END()

#endif  //! CCB_SQL_STORED_TIMESTAMP_HH
