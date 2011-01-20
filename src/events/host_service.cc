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

#include "events/host_service.hh"

using namespace events;

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
  action_url = hs.action_url;
  check_freshness = hs.check_freshness;
  default_active_checks_enabled = hs.default_active_checks_enabled;
  default_event_handler_enabled = hs.default_event_handler_enabled;
  default_failure_prediction = hs.default_failure_prediction;
  default_flap_detection_enabled = hs.default_flap_detection_enabled;
  default_notifications_enabled = hs.default_notifications_enabled;
  default_passive_checks_enabled = hs.default_passive_checks_enabled;
  default_process_perf_data = hs.default_process_perf_data;
  display_name = hs.display_name;
  first_notification_delay = hs.first_notification_delay;
  freshness_threshold = hs.freshness_threshold;
  high_flap_threshold = hs.high_flap_threshold;
  icon_image = hs.icon_image;
  icon_image_alt = hs.icon_image_alt;
  low_flap_threshold = hs.low_flap_threshold;
  notes = hs.notes;
  notes_url = hs.notes_url;
  notification_interval = hs.notification_interval;
  notification_period = hs.notification_period;
  notify_on_downtime = hs.notify_on_downtime;
  notify_on_flapping = hs.notify_on_flapping;
  notify_on_recovery = hs.notify_on_recovery;
  retain_nonstatus_information = hs.retain_nonstatus_information;
  retain_status_information = hs.retain_status_information;
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
host_service::host_service()
  : check_freshness(false),
    default_active_checks_enabled(false),
    default_event_handler_enabled(false),
    default_failure_prediction(false),
    default_flap_detection_enabled(false),
    default_notifications_enabled(false),
    default_passive_checks_enabled(false),
    default_process_perf_data(false),
    first_notification_delay(0.0),
    freshness_threshold(0.0),
    high_flap_threshold(0.0),
    low_flap_threshold(0.0),
    notification_interval(0.0),
    notify_on_downtime(0),
    notify_on_flapping(0),
    notify_on_recovery(0),
    retain_nonstatus_information(false),
    retain_status_information(false) {}

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
