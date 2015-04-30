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
