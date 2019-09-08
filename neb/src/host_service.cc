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
      default_notifications_enabled(false),
      default_passive_checks_enabled(false),
      first_notification_delay(0.0),
      freshness_threshold(0.0),
      high_flap_threshold(0.0),
      low_flap_threshold(0.0),
      notification_interval(0.0),
      notify_on_downtime(false),
      notify_on_flapping(false),
      notify_on_recovery(false),
      retain_nonstatus_information(false),
      retain_status_information(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the given object to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
host_service::host_service(host_service const& other) {
  _internal_copy(other);
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
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
host_service& host_service::operator=(host_service const& other) {
  _internal_copy(other);
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
 *  @param[in] other  Object to copy.
 */
void host_service::_internal_copy(host_service const& other) {
  action_url = other.action_url;
  check_freshness = other.check_freshness;
  default_active_checks_enabled = other.default_active_checks_enabled;
  default_event_handler_enabled = other.default_event_handler_enabled;
  default_flap_detection_enabled = other.default_flap_detection_enabled;
  default_notifications_enabled = other.default_notifications_enabled;
  default_passive_checks_enabled = other.default_passive_checks_enabled;
  display_name = other.display_name;
  first_notification_delay = other.first_notification_delay;
  freshness_threshold = other.freshness_threshold;
  high_flap_threshold = other.high_flap_threshold;
  icon_image = other.icon_image;
  icon_image_alt = other.icon_image_alt;
  low_flap_threshold = other.low_flap_threshold;
  notes = other.notes;
  notes_url = other.notes_url;
  notification_interval = other.notification_interval;
  notification_period = other.notification_period;
  notify_on_downtime = other.notify_on_downtime;
  notify_on_flapping = other.notify_on_flapping;
  notify_on_recovery = other.notify_on_recovery;
  retain_nonstatus_information = other.retain_nonstatus_information;
  retain_status_information = other.retain_status_information;
  return;
}
