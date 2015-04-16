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
#include "com/centreon/broker/neb/acknowledgement_removed.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

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
acknowledgement_removed::acknowledgement_removed()
  : host_id(0),
    service_id(0) {}

/**
 *  @brief acknowledgement copy constructor.
 *
 *  Copy data from the acknowledgement object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
acknowledgement_removed::acknowledgement_removed(acknowledgement_removed const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
acknowledgement_removed::~acknowledgement_removed() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
acknowledgement_removed& acknowledgement_removed::operator=(acknowledgement_removed const& other) {
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
unsigned int acknowledgement_removed::type() const {
  return (acknowledgement_removed::static_type());
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
void acknowledgement_removed::_internal_copy(acknowledgement_removed const& other) {
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
mapping::entry const acknowledgement_removed::entries[] = {
  mapping::entry(
    &acknowledgement_removed::host_id,
    "host_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &acknowledgement_removed::service_id,
    "service_id",
    2,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry()
};

// Operations.
static io::data* new_ack_removed() {
  return (new acknowledgement_removed);
}
io::event_info::event_operations const acknowledgement_removed::operations = {
  &new_ack_removed
};
