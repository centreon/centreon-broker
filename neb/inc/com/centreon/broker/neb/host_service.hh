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

#ifndef CCB_NEB_HOST_SERVICE_HH
#  define CCB_NEB_HOST_SERVICE_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         neb {
  /**
   *  @class host_service host_service.hh "com/centreon/broker/neb/host_service.hh"
   *  @brief Common class to host and service.
   *
   *  This class holds data members common to host and service
   *  classes.
   *
   *  @see host
   *  @see service
   */
  class           host_service {
  public:
                  host_service();
                  host_service(host_service const& other);
    virtual       ~host_service();
    host_service& operator=(host_service const& other);

    std::string       action_url;
    bool          check_freshness;
    bool          default_active_checks_enabled;
    bool          default_event_handler_enabled;
    bool          default_flap_detection_enabled;
    bool          default_notifications_enabled;
    bool          default_passive_checks_enabled;
    std::string       display_name;
    double        first_notification_delay;
    double        freshness_threshold;
    double        high_flap_threshold;
    std::string       icon_image;
    std::string       icon_image_alt;
    double        low_flap_threshold;
    std::string       notes;
    std::string       notes_url;
    double        notification_interval;
    std::string       notification_period;
    bool          notify_on_downtime;
    bool          notify_on_flapping;
    bool          notify_on_recovery;
    bool          retain_nonstatus_information;
    bool          retain_status_information;

  private:
    void          _internal_copy(host_service const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_SERVICE_HH
