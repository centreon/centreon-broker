/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/dumper/timestamp_cache.hh"
#include "com/centreon/broker/dumper/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Default constructor.
 */
timestamp_cache::timestamp_cache() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
timestamp_cache::timestamp_cache(timestamp_cache const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
timestamp_cache::~timestamp_cache() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
timestamp_cache& timestamp_cache::operator=(timestamp_cache const& right) {
  if (this != &right) {
    io::data::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int timestamp_cache::type() const {
  return (timestamp_cache::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int timestamp_cache::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_timestamp_cache>::value);
}


/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void timestamp_cache::_internal_copy(timestamp_cache const& right) {
  instance_id = right.instance_id;
  filename = right.filename;
  last_modified = right.last_modified;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const timestamp_cache::entries[] = {
  mapping::entry(
    &timestamp_cache::filename,
    "filename",
    1),
  mapping::entry(
    &timestamp_cache::last_modified,
    "last_modified",
    2),
  mapping::entry()
};

// Operations.
static io::data* new_timestamp_cache() {
  return (new timestamp_cache);
}
io::event_info::event_operations const timestamp_cache::operations = {
  &new_timestamp_cache
};
