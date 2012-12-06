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

#ifndef CCB_NEB_SERVICE_STATUS_HH
#  define CCB_NEB_SERVICE_STATUS_HH

#  include <QString>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/host_service_status.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace           neb {
  /**
   *  @class service_status service_status.hh "com/centreon/broker/neb/service_status.hh"
   *  @brief When the status of a service change, such an event is generated.
   *
   *  This class represents a change in a service status.
   */
  class             service_status : public host_service_status {
  public:
                    service_status();
                    service_status(service_status const& ss);
    virtual         ~service_status();
    service_status& operator=(service_status const& ss);
    QString const&  type() const;

    QString         host_name;
    timestamp       last_time_critical;
    timestamp       last_time_ok;
    timestamp       last_time_unknown;
    timestamp       last_time_warning;
    QString         service_description;
    unsigned int    service_id;

  private:
    void            _internal_copy(service_status const& ss);
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_STATUS_HH
