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

#include "com/centreon/broker/sql/stored_timestamp.hh"
#include <ctime>

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**
 *  Default constructor. Create a stored timestamp updated to now.
 */
stored_timestamp::stored_timestamp() throw()
    : _id(0), _ts(), _state(responsive) {
  update_timestamp();
}

/**
 *  Stored timestamp constructor.
 *
 *  @param id Id of the instance associated with this timestamp.
 *  @param s  State of the instance associated with this timestamp.
 */
stored_timestamp::stored_timestamp(unsigned int id, state_type s) throw()
    : _id(id), _state(s) {
  update_timestamp();
}

/**
 *  Default copy constructor.
 *
 *  @param[in] right Te stored_timestamp to copy.
 */
stored_timestamp::stored_timestamp(stored_timestamp const& right) throw()
    : _id(right._id), _ts(right._ts), _state(right._state) {}

/**
 *  Destructor.
 */
stored_timestamp::~stored_timestamp() throw() {}

/**
 *  Get the the id of the instance associated with this timestamp.
 *
 *  @return The id of the instance associated with this timestamp.
 */
unsigned int stored_timestamp::get_id() const throw() {
  return (_id);
}

/**
 *  Get the state of the instance associated with this timestamp.
 *
 *  @return The state of the instance associated with this timestamp.
 */
stored_timestamp::state_type stored_timestamp::get_state() const throw() {
  return (_state);
}

/**
 *  Set the state of the instance associated with this timestamp.
 *
 *  @param[in] state the new state.
 */
void stored_timestamp::set_state(state_type state) throw() {
  _state = state;
}

/**
 *  Update the internal timestamp to now.
 */
void stored_timestamp::update_timestamp() throw() {
  _ts = timestamp(std::time(NULL));
}

/**
 *  Get the internal timestamp.
 *
 *  @return The timestamp.
 */
timestamp stored_timestamp::get_timestamp() const throw() {
  return (_ts);
}

/**
 *  Set the internal timestamp.
 *
 *  @param[in] ts The timestamp to set.
 */
void stored_timestamp::set_timestamp(timestamp ts) throw() {
  _ts = ts;
}

/**
 * Check to see if the timestamp is outdated.
 *
 * @param[in] timeout The timeout, in seconds.
 *
 * @return true if the timestamp is outdated.
 */
bool stored_timestamp::timestamp_outdated(unsigned int timeout) const throw() {
  return (std::difftime(std::time(NULL), _ts) > timeout);
}
