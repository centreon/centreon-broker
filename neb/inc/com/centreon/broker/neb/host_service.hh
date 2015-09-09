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

#ifndef CCB_NEB_HOST_SERVICE_HH
#  define CCB_NEB_HOST_SERVICE_HH

#  include <QString>
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
                  host_service(host_service const& hs);
    virtual       ~host_service();
    host_service& operator=(host_service const& hs);

    bool          check_freshness;
    bool          default_active_checks_enabled;
    bool          default_event_handler_enabled;
    bool          default_flap_detection_enabled;
    double        freshness_threshold;
    double        high_flap_threshold;
    double        low_flap_threshold;

  private:
    void          _internal_copy(host_service const& hs);
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_SERVICE_HH
