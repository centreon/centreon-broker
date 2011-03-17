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

#ifndef EVENTS_INSTANCE_HH_
# define EVENTS_INSTANCE_HH_

# include <string>
# include <sys/types.h>
# include "events/event.hh"

namespace       events {
  /**
   *  @class instance instance.hh "events/instance.hh"
   *  @brief Information about Nagios process.
   *
   *  This class holds information about a Nagios process, like whether
   *  it is running or not, in daemon mode or not, ...
   */
  class         instance : public event {
   private:
    void        _internal_copy(instance const& i);

   public:
    std::string engine;
    int         id;
    bool        is_running;
    std::string name;
    int         pid;
    time_t      program_end;
    time_t      program_start;
    std::string version;
                instance();
                instance(instance const& i);
                ~instance();
    instance&   operator=(instance const& i);
    int         get_type() const;
  };
}

#endif /* !EVENTS_INSTANCE_HH_ */
