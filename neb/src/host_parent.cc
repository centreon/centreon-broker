/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/neb/host_parent.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
host_parent::host_parent() : enabled(true), host_id(0), parent_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
host_parent::host_parent(host_parent const& other)
    : io::data(other),
      enabled(other.enabled),
      host_id(other.host_id),
      parent_id(other.parent_id) {}

/**
 *  Destructor.
 */
host_parent::~host_parent() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_parent& host_parent::operator=(host_parent const& other) {
  if (this != &other) {
    io::data::operator=(other);
    enabled = other.enabled;
    host_id = other.host_id;
    parent_id = other.parent_id;
  }
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int host_parent::type() const {
  return (host_parent::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int host_parent::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_host_parent>::value);
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const host_parent::entries[] = {
    mapping::entry(&host_parent::enabled, ""),
    mapping::entry(&host_parent::host_id,
                   "child_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&host_parent::parent_id,
                   "parent_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry()};

// Operations.
static io::data* new_host_parent() {
  return (new host_parent);
}
io::event_info::event_operations const host_parent::operations = {
    &new_host_parent};
