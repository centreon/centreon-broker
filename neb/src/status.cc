/*
** Copyright 2009-2011 Merethis
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

#include "com/centreon/broker/neb/status.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy members of the given Status object to the current
 *         instance.
 *
 *  Copy all members defined within the status class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  @param[in] s Object to copy.
 */
void status::_internal_copy(status const& s) {
  event_handler_enabled = s.event_handler_enabled;
  failure_prediction_enabled = s.failure_prediction_enabled;
  flap_detection_enabled = s.flap_detection_enabled;
  notifications_enabled = s.notifications_enabled;
  process_performance_data = s.process_performance_data;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
status::status()
  : event_handler_enabled(false),
    failure_prediction_enabled(false),
    flap_detection_enabled(false),
    notifications_enabled(false),
    process_performance_data(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given status object to the current instance.
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
 *  @brief Assignment operator.
 *
 *  Copy all members of the given status object to the current instance.
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
