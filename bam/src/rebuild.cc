/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
rebuild::rebuild() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
rebuild::rebuild(rebuild const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
rebuild::~rebuild() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
rebuild& rebuild::operator=(rebuild const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test for equality.
 *
 *  @return  True if the two objects are equal.
 */
bool rebuild::operator==(rebuild const& other) const {
  return (bas_to_rebuild == other.bas_to_rebuild);
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int rebuild::type() const {
  return (rebuild::static_type());
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int rebuild::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_rebuild>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void rebuild::_internal_copy(rebuild const& other) {
  bas_to_rebuild = other.bas_to_rebuild;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const rebuild::entries[] = {
    mapping::entry(&bam::rebuild::bas_to_rebuild, "bas_to_rebuild"),
    mapping::entry()};

// Operations.
static io::data* new_rebuild() {
  return (new rebuild);
}
io::event_info::event_operations const rebuild::operations = {&new_rebuild};
