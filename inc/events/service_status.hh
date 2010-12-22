/*
** Copyright 2009-2010 MERETHIS
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

#ifndef EVENTS_SERVICE_STATUS_HH_
# define EVENTS_SERVICE_STATUS_HH_

# include <string>
# include <sys/types.h>
# include "events/host_service_status.hh"

namespace               events {
  // Forward declaration.
  class                 event_subscriber;

  /**
   *  @class service_status service_status.hh "events/service_status.hh"
   *  @brief When the status of a service change, such an event is generated.
   *
   *  This class represents a change in a service status.
   */
  class                service_status : public host_service_status {
   private:
    void               _internal_copy(service_status const& ss);

   public:
    time_t             last_time_critical;
    time_t             last_time_ok;
    time_t             last_time_unknown;
    time_t             last_time_warning;
    int                service_id;
                       service_status();
                       service_status(service_status const& ss);
    virtual            ~service_status();
    service_status&    operator=(service_status const& ss);
    int                get_type() const;
  };
}

#endif /* !EVENTS_SERVICE_STATUS_HH_ */
