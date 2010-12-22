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

#ifndef EVENTS_DOWNTIME_HH_
# define EVENTS_DOWNTIME_HH_

# include <string>
# include <sys/time.h>
# include "events/event.hh"

namespace              events {
  /**
   *  @class downtime downtime.hh "events/downtime.hh"
   *  @brief Represents a downtime inside Nagios.
   *
   *  A Nagios user may have the ability to define downtimes, which are
   *  time periods inside which some host or service shall not generate
   *  any notification. This can occur when an admin sys perform
   *  maintenance on a server for example.
   */
  class                downtime : public event {
   private:
    void               _internal_copy(downtime const& d);

   public:
    std::string        author;
    std::string        comment;
    short              downtime_type;
    time_t             duration;
    time_t             end_time;
    time_t             entry_time;
    bool               fixed;
    std::string        host_name;
    int                internal_id;
    std::string        instance_name;
    std::string        service_description;
    time_t             start_time;
    int                triggered_by;
    bool               was_cancelled;
    bool               was_started;
                       downtime();
                       downtime(downtime const& d);
                       ~downtime();
    downtime&          operator=(downtime const& d);
    int                get_type() const;
  };
}

#endif /* !EVENTS_DOWNTIME_HH_ */
