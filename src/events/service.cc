/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "events/service.hh"

using namespace events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy members of the service object to the current instance.
 *
 *  Copy all members defined within the service class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  @param[in] s Object to copy.
 */
void service::_internal_copy(service const& s) {
  failure_prediction_options = s.failure_prediction_options;
  flap_detection_on_critical = s.flap_detection_on_critical;
  flap_detection_on_ok = s.flap_detection_on_ok;
  flap_detection_on_unknown = s.flap_detection_on_unknown;
  is_volatile = s.is_volatile;
  notify_on_critical = s.notify_on_critical;
  notify_on_unknown = s.notify_on_unknown;
  notify_on_warning = s.notify_on_warning;
  service_description = s.service_description;
  stalk_on_critical = s.stalk_on_critical;
  stalk_on_unknown = s.stalk_on_unknown;
  stalk_on_warning = s.stalk_on_warning;
  return ;
}

/**
 *  @brief Initialize members to 0, NULL or equivalent.
 *
 *  This method initializes members defined within the Service class to
 *  0, NULL or equivalent. This method is used by some constructors.
 */
void service::_zero_initialize() {
  flap_detection_on_critical = false;
  flap_detection_on_ok = false;
  flap_detection_on_unknown = false;
  flap_detection_on_warning = false;
  is_volatile = false;
  notify_on_critical = false;
  notify_on_unknown = false;
  notify_on_warning = false;
  stalk_on_critical = false;
  stalk_on_ok = false;
  stalk_on_unknown = false;
  stalk_on_warning = false;
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
service::service() {
  _zero_initialize();
}

/**
 *  @brief Build a service from a service_status.
 *
 *  Copy all members from service_status to the current instance and
 *  zero-initialize remaining members.
 *
 *  @param[in] ss Object to copy.
 */
service::service(service_status const& ss) : service_status(ss) {
  _zero_initialize();
}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given service object to the current
 *  instance.
 *
 *  @param[in] s Object to copy.
 */
service::service(service const& s)
  : host_service(s), service_status(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
service::~service() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given service object to the current
 *  instance.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
service& service::operator=(service const& s) {
  host_service::operator=(s);
  service_status::operator=(s);
  _internal_copy(s);
  return (*this);
}

/**
 *  @brief Get the type of the event (event::SERVICE).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  @return event::SERVICE
 */
int service::get_type() const {
  return (SERVICE);
}
