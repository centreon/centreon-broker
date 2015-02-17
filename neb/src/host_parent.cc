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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/host_parent.hh"
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
host_parent::host_parent()
  : enabled(true),
    host_id(0),
    parent_id(0) {}

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
  mapping::entry(
    &host_parent::enabled,
    "",
    1),
  mapping::entry(
    &host_parent::host_id,
    "child_id",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host_parent::parent_id,
    "parent_id",
    3,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &host_parent::instance_id,
    "",
    4),
  mapping::entry()
};

// Operations.
static io::data* new_host_parent() {
  return (new host_parent);
}
io::event_info::event_operations const host_parent::operations = {
  &new_host_parent
};
