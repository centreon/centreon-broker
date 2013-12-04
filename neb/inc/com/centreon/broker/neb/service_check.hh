/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_SERVICE_CHECK_HH
#  define CCB_NEB_SERVICE_CHECK_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/check.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class service_check service_check.hh "com/centreon/broker/neb/service_check.hh"
   *  @brief Check that has been executed on a service.
   *
   *  Once a check has been executed on a service, an object of
   *  this class is sent.
   */
  class            service_check : public check {
  public:
                   service_check();
                   service_check(service_check const& sc);
    virtual        ~service_check();
    service_check& operator=(service_check const& sc);
    unsigned int   type() const;

    unsigned int   service_id;
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_CHECK_HH
