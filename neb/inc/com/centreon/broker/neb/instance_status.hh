/*
** Copyright 2009-2012,2015 Merethis
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
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
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
                     instance_status(instance_status const& other);
                     ~instance_status();
    instance_status& operator=(instance_status const& other);
    unsigned int     type() const;
    static unsigned int
                     static_type();

    QString          address;
    bool             check_hosts_freshness;
    bool             check_services_freshness;
    QString          description;
    QString          global_host_event_handler;
    QString          global_service_event_handler;
    unsigned int     id;
    timestamp        last_alive;
    timestamp        last_command_check;
    bool             obsess_over_hosts;
    bool             obsess_over_services;

    static mapping::entry const
                     entries[];
    static io::event_info::event_operations const
                     operations;

  private:
    void             _internal_copy(instance_status const& other);
  };
}

CCB_END()

#endif // !CCB_EVENTS_INSTANCE_STATUS_HH
