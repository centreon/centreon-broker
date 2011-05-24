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
*/

#ifndef CCB_EVENTS_HOST_HH_
# define CCB_EVENTS_HOST_HH_

# include <QString>
# include "events/host_service.hh"
# include "events/host_status.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   events {
        /**
         *  @class host host.hh "events/host.hh"
         *  @brief Host within Nagios.
         *
         *  Nagios has two main objects that can be manipulated : host and
         *  service. An host object holds every parameter related to a host
         *  machine.
         */
        class     host : public host_service, public host_status {
         private:
          void    _internal_copy(host const& h);
          void    _zero_initialize();

         public:
          QString address;
          QString alias;
          bool    flap_detection_on_down;
          bool    flap_detection_on_unreachable;
          bool    flap_detection_on_up;
          QString host_name;
          int     instance_id;
          bool    notify_on_down;
          bool    notify_on_unreachable;
          bool    stalk_on_down;
          bool    stalk_on_unreachable;
          bool    stalk_on_up;
          QString statusmap_image;
                  host();
                  host(host_status const& hs);
                  host(host const& h);
                  ~host();
          host&   operator=(host const& h);
          int     get_type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_HOST_HH_ */
