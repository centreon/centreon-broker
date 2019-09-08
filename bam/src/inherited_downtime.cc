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

#include "com/centreon/broker/bam/inherited_downtime.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
inherited_downtime::inherited_downtime() : ba_id(0), in_downtime(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
inherited_downtime::inherited_downtime(inherited_downtime const& other)
    : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
inherited_downtime::~inherited_downtime() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
inherited_downtime& inherited_downtime::operator=(
    inherited_downtime const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int inherited_downtime::type() const {
  return (inherited_downtime::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int inherited_downtime::static_type() {
  return (io::events::data_type<io::events::bam,
                                bam::de_inherited_downtime>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void inherited_downtime::_internal_copy(inherited_downtime const& other) {
  ba_id = other.ba_id;
  in_downtime = other.in_downtime;
  return;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const inherited_downtime::entries[] = {
    mapping::entry(&bam::inherited_downtime::ba_id,
                   "ba_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&bam::inherited_downtime::in_downtime, "in_downtime"),
    mapping::entry()};

// Operations.
static io::data* new_inherited_downtime() {
  return (new inherited_downtime);
}
io::event_info::event_operations const inherited_downtime::operations = {
    &new_inherited_downtime};
