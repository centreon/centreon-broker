/*
** Copyright 2014-2015 Merethis
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
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/meta_service_status.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
meta_service_status::meta_service_status()
  : meta_service_id(0), state_changed(false), value(NAN) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
meta_service_status::meta_service_status(
                       meta_service_status const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
meta_service_status::~meta_service_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
meta_service_status& meta_service_status::operator=(
                                            meta_service_status const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int meta_service_status::type() const {
  return (meta_service_status::static_type());
}

/**
 *  Get event type.
 *
 *  @return Event type.
 */
unsigned int meta_service_status::static_type() {
  return (io::events::data_type<io::events::bam, bam::de_meta_service_status>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void meta_service_status::_internal_copy(
                            meta_service_status const& other) {
  meta_service_id = other.meta_service_id;
  state_changed = other.state_changed;
  value = other.value;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const meta_service_status::entries[] = {
  mapping::entry(
    &bam::meta_service_status::meta_service_id,
    "meta_service_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &bam::meta_service_status::value,
    "value"),
  mapping::entry(
    &bam::meta_service_status::state_changed,
    ""),
  mapping::entry()
};

// Operations.
static io::data* new_meta_service_status() {
  return (new meta_service_status);
}
io::event_info::event_operations const meta_service_status::operations = {
  &new_meta_service_status
};
