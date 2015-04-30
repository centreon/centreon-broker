/*
** Copyright 2009-2013,2015 Merethis
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
#include "com/centreon/broker/storage/index_mapping.hh"

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
index_mapping::index_mapping()
  : index_id(0),
    host_id(0),
    service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
index_mapping::index_mapping(index_mapping const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
index_mapping::~index_mapping() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
index_mapping& index_mapping::operator=(index_mapping const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int index_mapping::type() const {
  return (index_mapping::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int index_mapping::static_type() {
  return (io::events::data_type<io::events::storage, storage::de_index_mapping>::value);
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
void index_mapping::_internal_copy(index_mapping const& other) {
  index_id = other.index_id;
  host_id = other.host_id;
  service_id = other.service_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const index_mapping::entries[] = {
  mapping::entry(
    &index_mapping::index_id,
    "index_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &index_mapping::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &index_mapping::service_id,
    "service_id",
    mapping::entry::invalid_on_zero),
  mapping::entry()
};

// Operations.
static io::data* new_index_mapping() {
  return (new index_mapping);
}
io::event_info::event_operations const index_mapping::operations = {
  &new_index_mapping
};
