/*
** Copyright 2009-2013 Merethis
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

#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/service_state.hh"

using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_state::service_state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
service_state::service_state(service_state const& ss) : state(ss) {}

/**
 *  Destructor.
 */
service_state::~service_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_state& service_state::operator=(service_state const& ss) {
  state::operator=(ss);
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] ss Object to compare to.
 *
 *  @return true if both objects are equal.
 */
bool service_state::operator==(service_state const& ss) const {
  return ((this == &ss)
          || ((ack_time == ss.ack_time)
              && (current_state == ss.current_state)
              && (end_time == ss.end_time)
              && (host_id == ss.host_id)
              && (in_downtime == ss.in_downtime)
              && (service_id == ss.service_id)
              && (start_time == ss.start_time)));
}

/**
 *  Non-equality operator.
 *
 *  @param[in] ss Object to compare to.
 *
 *  @return true if both objects are not equal.
 */
bool service_state::operator!=(service_state const& ss) const {
  return (!this->operator==(ss));
}

/**
 *  Get the type of this object.
 *
 *  @return The event type.
 */
unsigned int service_state::type() const {
  return (io::data::data_type(io::data::correlation, correlation::de_service_state));
}
