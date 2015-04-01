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

#include "com/centreon/broker/notification/internal.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/notification/downtime_removed.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief acknowledgement default constructor.
 *
 *  acknowledgement default constructor. Set all members to their
 *  default value (0, NULL or equivalent).
 */
downtime_removed::downtime_removed()
  : downtime_id(0) {}

/**
 *  @brief acknowledgement copy constructor.
 *
 *  Copy data from the acknowledgement object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
downtime_removed::downtime_removed(downtime_removed const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
downtime_removed::~downtime_removed() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
downtime_removed& downtime_removed::operator=(downtime_removed const& other) {
  io::data::operator=(other);
  _internal_copy(other);
  return (*this);
}

/**
 *  @brief Get the type of the event.
 *
 *  Return the type of this event. This can be useful for runtime data
 *  type determination.
 *
 *  @return The event type.
 */
unsigned int downtime_removed::type() const {
  return (downtime_removed::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int downtime_removed::static_type() {
  return (io::events::data_type<io::events::notification, notification::de_downtime_removed>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the given object to the current
 *         instance.
 */
void downtime_removed::_internal_copy(downtime_removed const& other) {
  downtime_id = other.downtime_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const downtime_removed::entries[] = {
  mapping::entry(
    &downtime_removed::downtime_id,
    "downtime_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry()
};

// Operations.
static io::data* new_downtime_removed() {
  return (new downtime_removed);
}
io::event_info::event_operations const downtime_removed::operations = {
  &new_downtime_removed
};
