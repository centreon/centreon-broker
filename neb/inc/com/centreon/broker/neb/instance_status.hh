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

#ifndef CCB_EVENTS_INSTANCE_STATUS_HH
#  define CCB_EVENTS_INSTANCE_STATUS_HH

#  include <QString>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/status.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace            neb {
  /**
   *  @class instance_status instance_status.hh "com/centreon/broker/neb/instance_status.hh"
   *  @brief Information about Nagios process.
   *
   *  instance_status holds information about a scheduling
   *  process, like whether it is running or not, in daemon mode
   *  or not, ...
   */
  class              instance_status : public status {
  public:
                     instance_status();
                     instance_status(instance_status const& is);
                     ~instance_status();
    instance_status& operator=(instance_status const& is);
    unsigned int     type() const;

    bool             active_host_checks_enabled;
    bool             active_service_checks_enabled;
    QString          address;
    bool             check_hosts_freshness;
    bool             check_services_freshness;
    bool             daemon_mode;
    QString          description;
    QString          global_host_event_handler;
    QString          global_service_event_handler;
    unsigned int     id;
    timestamp        last_alive;
    timestamp        last_command_check;
    timestamp        last_log_rotation;
    int              modified_host_attributes;
    int              modified_service_attributes;
    bool             obsess_over_hosts;
    bool             obsess_over_services;
    bool             passive_host_checks_enabled;
    bool             passive_service_checks_enabled;

  private:
    void             _internal_copy(instance_status const& is);
  };
}

CCB_END()

#endif // !CCB_EVENTS_INSTANCE_STATUS_HH
