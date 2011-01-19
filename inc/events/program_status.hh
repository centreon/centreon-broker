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

#ifndef EVENTS_PROGRAM_STATUS_HH_
# define EVENTS_PROGRAM_STATUS_HH_

# include <string>
# include <sys/types.h>
# include "events/status.hh"

namespace              events {
  /**
   *  @class program_status program_status.hh "events/program_status.hh"
   *  @brief Information about Nagios process.
   *
   *  program_status holds information about a Nagios process, like
   *  whether it is running or not, in daemon mode or not, ...
   */
  class                program_status : public status {
   private:
    void               _internal_copy(program_status const& ps);

   public:
    bool               active_host_checks_enabled;
    bool               active_service_checks_enabled;
    bool               check_hosts_freshness;
    bool               check_services_freshness;
    bool               daemon_mode;
    std::string        global_host_event_handler;
    std::string        global_service_event_handler;
    std::string        instance_address;
    std::string        instance_description;
    int                instance_id;
    time_t             last_alive;
    time_t             last_command_check;
    time_t             last_log_rotation;
    int                modified_host_attributes;
    int                modified_service_attributes;
    bool               obsess_over_hosts;
    bool               obsess_over_services;
    bool               passive_host_checks_enabled;
    bool               passive_service_checks_enabled;
                       program_status();
                       program_status(program_status const& ps);
                       ~program_status();
    program_status&    operator=(program_status const& ps);
    int                get_type() const;
  };
}

#endif /* !EVENTS_PROGRAM_STATUS_HH_ */
