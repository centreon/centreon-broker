/*
** Copyright 2009-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/status_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
status_mapping::status_mapping()
  : index_id(0),
    host_id(0),
    service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] m Object to copy.
 */
status_mapping::status_mapping(status_mapping const& m) : io::data(m) {
  _internal_copy(m);
}

/**
 *  Destructor.
 */
status_mapping::~status_mapping() {}

/**
 *  Assignment operator.
 *
 *  @param[in] m Object to copy.
 *
 *  @return This object.
 */
status_mapping& status_mapping::operator=(status_mapping const& m) {
  io::data::operator=(m);
  _internal_copy(m);
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int status_mapping::type() const {
  return (status_mapping::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int status_mapping::static_type() {
  return (io::events::data_type<io::events::storage, storage::de_status_mapping>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] m Object to copy.
 */
void status_mapping::_internal_copy(status_mapping const& m) {
  index_id = m.index_id;
  host_id = m.host_id;
  service_id = m.service_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const status_mapping::entries[] = {
  mapping::entry(
    &status_mapping::index_id,
    "index_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &status_mapping::host_id,
    "host_id",
    1,
  mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &status_mapping::service_id,
    "service_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry()
};

// Operations.
static io::data* new_status_mapping() {
  return (new status_mapping);
}
io::event_info::event_operations const status_mapping::operations = {
  &new_status_mapping
};
