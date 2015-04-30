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
#include "com/centreon/broker/neb/host_check.hh"
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
host_check::host_check() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
host_check::host_check(host_check const& other) : check(other) {}

/**
 *  Destructor.
 */
host_check::~host_check() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_check& host_check::operator=(host_check const& other) {
  check::operator=(other);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host_check::type() const {
  return (host_check::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int host_check::static_type() {
  return (io::events::data_type<io::events::neb, neb::de_host_check>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const host_check::entries[] = {
  mapping::entry(
    &host_check::active_checks_enabled,
    ""),
  mapping::entry(
    &host_check::check_type,
    ""),
  mapping::entry(
    &host_check::host_id,
    "host_id",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &host_check::next_check,
    ""),
  mapping::entry(
    &host_check::command_line,
    "command_line"),
  mapping::entry()
};

// Operations.
static io::data* new_host_check() {
  return (new host_check);
}
io::event_info::event_operations const host_check::operations = {
  &new_host_check
};
