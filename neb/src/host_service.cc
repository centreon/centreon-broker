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

#include "com/centreon/broker/neb/host_service.hh"

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
host_service::host_service()
  : check_freshness(false),
    default_active_checks_enabled(false),
    default_event_handler_enabled(false),
    default_flap_detection_enabled(false),
    freshness_threshold(0.0),
    high_flap_threshold(0.0),
    low_flap_threshold(0.0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  @param[in] hs Object to copy.
 */
host_service::host_service(host_service const& hs) {
  _internal_copy(hs);
}

/**
 *  Destructor.
 */
host_service::~host_service() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  @param[in] hs Object to copy.
 *
 *  @return This object.
 */
host_service& host_service::operator=(host_service const& hs) {
  _internal_copy(hs);
  return (*this);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy all internal data of the host_service object to the
 *         current instance.
 *
 *  Copy data defined within the host_service class. This is used by the
 *  copy constructor and the assignment operator.
 *
 *  @param[in] hs Object to copy.
 */
void host_service::_internal_copy(host_service const& hs) {
  check_freshness = hs.check_freshness;
  default_active_checks_enabled = hs.default_active_checks_enabled;
  default_event_handler_enabled = hs.default_event_handler_enabled;
  default_flap_detection_enabled = hs.default_flap_detection_enabled;
  freshness_threshold = hs.freshness_threshold;
  high_flap_threshold = hs.high_flap_threshold;
  low_flap_threshold = hs.low_flap_threshold;
  return ;
}
