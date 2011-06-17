/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NEB_SERVICE_STATUS_HH_
# define CCB_NEB_SERVICE_STATUS_HH_

# include <time.h>
# include "com/centreon/broker/neb/host_service_status.hh"

namespace                 com {
  namespace               centreon {
    namespace             broker {
      namespace           neb {
        /**
         *  @class service_status service_status.hh "com/centreon/broker/neb/service_status.hh"
         *  @brief When the status of a service change, such an event is generated.
         *
         *  This class represents a change in a service status.
         */
        class             service_status : public host_service_status {
         private:
          void            _internal_copy(service_status const& ss);

         public:
          time_t          last_time_critical;
          time_t          last_time_ok;
          time_t          last_time_unknown;
          time_t          last_time_warning;
          unsigned int    service_id;
                          service_status();
                          service_status(service_status const& ss);
          virtual         ~service_status();
          service_status& operator=(service_status const& ss);
          QString const&  type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_SERVICE_STATUS_HH_ */
