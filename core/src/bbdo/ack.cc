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

#include "com/centreon/broker/bbdo/ack.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
ack::ack() : io::data(ack::static_type()), acknowledged_events(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ack::ack(ack const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ack::~ack() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ack& ack::operator=(ack const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return *this;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void ack::_internal_copy(ack const& other) {
  acknowledged_events = other.acknowledged_events;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const ack::entries[] = {
    mapping::entry(&ack::acknowledged_events, "acknowledged_events"),
    mapping::entry()};

// Operations.
static io::data* new_ack() {
  return new ack;
}
io::event_info::event_operations const ack::operations = {&new_ack};
