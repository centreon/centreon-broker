/*
** Copyright 2009-2013,2015 Centreon
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

#include "com/centreon/broker/neb/status.hh"

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
status::status(uint32_t type)
    : io::data(type),
      event_handler_enabled(false),
      flap_detection_enabled(false),
      notifications_enabled(false) {}

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
  flap_detection_enabled = s.flap_detection_enabled;
  notifications_enabled = s.notifications_enabled;
  return;
}
