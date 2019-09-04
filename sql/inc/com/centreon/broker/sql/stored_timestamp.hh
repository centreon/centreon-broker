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

namespace sql {
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

  stored_timestamp() throw();
  stored_timestamp(unsigned int id, state_type s) throw();
  stored_timestamp(stored_timestamp const& right) throw();
  ~stored_timestamp() throw();

  unsigned int get_id() const throw();
  state_type get_state() const throw();
  void set_state(state_type state) throw();
  void update_timestamp() throw();
  timestamp get_timestamp() const throw();
  void set_timestamp(timestamp ts) throw();
  bool timestamp_outdated(unsigned int timeout) const throw();

 private:
  unsigned int _id;
  timestamp _ts;
  state_type _state;
};
}  // namespace sql

CCB_END()

#endif  //! CCB_SQL_STORED_TIMESTAMP_HH
