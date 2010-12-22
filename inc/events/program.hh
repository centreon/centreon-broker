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

#ifndef EVENTS_PROGRAM_HH_
# define EVENTS_PROGRAM_HH_

# include <string>
# include <sys/types.h>
# include "events/event.hh"

namespace       events
{
  /**
   *  @class program program.hh "events/program.hh"
   *  @brief Information about Nagios process.
   *
   *  Program holds information about a Nagios process, like whether it
   *  is running or not, in daemon mode or not, ...
   */
  class         program : public event {
   private:
    void        _internal_copy(program const& p);

   public:
    bool        daemon_mode;
    int         instance_id;
    std::string instance_name;
    bool        is_running;
    int         pid;
    time_t      program_end;
    time_t      program_start;
                program();
                program(program const& p);
                ~program();
    program&    operator=(program const& p);
    int         get_type() const;
  };
}

#endif /* !EVENTS_PROGRAM_HH_ */
