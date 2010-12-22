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

#ifndef EVENTS_SERVICE_CHECK_HH_
# define EVENTS_SERVICE_CHECK_HH_

# include <string>
# include "events/check.hh"

namespace          events {
  /**
   *  @class service_check service_check.hh "events/service_check.hh"
   *  @brief Check that has been executed on a service.
   *
   *  Once a check has been executed on a service, an object of this
   *  class is sent.
   */
  class            service_check : public check {
   public:
    int            service_id;
                   service_check();
                   service_check(service_check const& sc);
    virtual        ~service_check();
    service_check& operator=(service_check const& sc);
    int            get_type() const;
  };
}

#endif /* !EVENTS_SERVICE_CHECK_HH_ */
