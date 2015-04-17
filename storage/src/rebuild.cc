/*
** Copyright 2012-2013,2015 Merethis
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
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/rebuild.hh"

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
rebuild::rebuild() : end(true), id(0), is_index(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
rebuild::rebuild(rebuild const& right) : io::data(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
rebuild::~rebuild() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
rebuild& rebuild::operator=(rebuild const& right) {
  if (this != &right) {
    io::data::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int rebuild::type() const {
  return (rebuild::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int rebuild::static_type() {
  return (io::events::data_type<io::events::storage, storage::de_rebuild>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void rebuild::_internal_copy(rebuild const& right) {
  end = right.end;
  id = right.id;
  is_index = right.is_index;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const rebuild::entries[] = {
  mapping::entry(
    &rebuild::end,
    "end"),
  mapping::entry(
    &rebuild::id,
    "id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &rebuild::is_index,
    "is_index"),
  mapping::entry()
};

// Operations.
static io::data* new_rebuild() {
  return (new rebuild);
}
io::event_info::event_operations const rebuild::operations = {
  &new_rebuild
};
