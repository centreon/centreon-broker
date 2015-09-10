/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service.hh"

using namespace com::centreon::broker::neb;

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
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
unsigned int service::type() const {
  return (io::events::data_type<io::events::neb, neb::de_service>::value);
}

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
  flap_detection_on_warning = s.flap_detection_on_warning;
  is_volatile = s.is_volatile;
  notify_on_critical = s.notify_on_critical;
  notify_on_unknown = s.notify_on_unknown;
  notify_on_warning = s.notify_on_warning;
  stalk_on_critical = s.stalk_on_critical;
  stalk_on_ok = s.stalk_on_ok;
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
