/*
** Copyright 2014 Merethis
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
rebuild::rebuild(
    rebuild const& other)
  : io::data(other) {
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
rebuild& rebuild::operator=(
    rebuild const& other) {
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
bool rebuild::operator==(
    rebuild const& other) const {
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
  return (io::events::data_type<io::events::bam,
                                bam::de_rebuild>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void rebuild::_internal_copy(
    rebuild const& other) {
  bas_to_rebuild = other.bas_to_rebuild;
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
    &bam::rebuild::bas_to_rebuild,
    "bas_to_rebuild",
    1),
  mapping::entry(
    &bam::rebuild::instance_id,
    "",
    2),
  mapping::entry()
};

// Operations.
static io::data* new_rebuild() {
  return (new rebuild);
}
io::event_info::event_operations const rebuild::operations = {
  &new_rebuild
};
