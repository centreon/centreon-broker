/*
** Copyright 2011-2013 Merethis
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
#include "com/centreon/broker/storage/status.hh"

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
status::status()
  : ctime(0),
    index_id(0),
    interval(0),
    is_for_rebuild(false),
    rrd_len(0),
    state(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
status::status(status const& s) : io::data(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
status::~status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
status& status::operator=(status const& s) {
  io::data::operator=(s);
  _internal_copy(s);
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int status::type() const {
  return (status::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int status::static_type() {
  return (io::events::data_type<io::events::storage, storage::de_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] s Object to copy.
 */
void status::_internal_copy(status const& s) {
  ctime = s.ctime;
  index_id = s.index_id;
  interval = s.interval;
  is_for_rebuild = s.is_for_rebuild;
  rrd_len = s.rrd_len;
  state = s.state;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const status::entries[] = {
  mapping::entry(
    &status::ctime,
    "ctime",
    1),
  mapping::entry(
    &status::index_id,
    "index_id",
    2),
  mapping::entry(
    &status::interval,
    "interval",
    3),
  mapping::entry(
    &status::rrd_len,
    "rrd_len",
    4),
  mapping::entry(
    &status::state,
    "state",
    5),
  mapping::entry(
    &status::is_for_rebuild,
    "is_for_rebuild",
    6),
  mapping::entry()
};

// Operations.
static io::data* new_status() {
  return (new status);
}
io::event_info::event_operations const status::operations = {
  &new_status
};
