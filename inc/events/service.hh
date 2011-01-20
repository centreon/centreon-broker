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

#ifndef EVENTS_SERVICE_HH_
# define EVENTS_SERVICE_HH_

# include <string>
# include <sys/types.h>
# include "events/host_service.hh"
# include "events/service_status.hh"

namespace              events {
  /**
   *  @class service service.hh "events/service.hh"
   *  @brief Service as handled by Nagios.
   *
   *  Holds full data regarding a service.
   *
   *  @see host_service
   *  @see service_status
   */
  class                service : public host_service,
                                 public service_status {
   private:
    void               _internal_copy(service const& s);
    void               _zero_initialize();

   public:
    std::string        failure_prediction_options;
    bool               flap_detection_on_critical;
    bool               flap_detection_on_ok;
    bool               flap_detection_on_unknown;
    bool               flap_detection_on_warning;
    bool               is_volatile;
    bool               notify_on_critical;
    bool               notify_on_unknown;
    bool               notify_on_warning;
    std::string        service_description;
    bool               stalk_on_critical;
    bool               stalk_on_ok;
    bool               stalk_on_unknown;
    bool               stalk_on_warning;
                       service();
                       service(service_status const& ss);
                       service(service const& s);
                       ~service();
    service&           operator=(service const& s);
    int                get_type() const;
  };
}

#endif /* !EVENTS_SERVICE_HH_ */
