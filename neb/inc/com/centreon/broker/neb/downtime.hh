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

#ifndef CCB_NEB_DOWNTIME_HH
#  define CCB_NEB_DOWNTIME_HH

#  include <QString>
#  include <ctime>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class downtime downtime.hh "com/centreon/broker/neb/downtime.hh"
   *  @brief Represents a downtime inside Nagios.
   *
   *  A user may have the ability to define downtimes, which are
   *  time periods inside which some host or service shall not
   *  generate any notification. This can occur when a system
   *  administrator perform maintenance on a server for example.
   */
  class            downtime : public io::data {
  public:
                   downtime();
                   downtime(downtime const& d);
                   ~downtime();
    downtime&      operator=(downtime const& d);
    QString const& type() const;

    QString        author;
    QString        comment;
    time_t         deletion_time;
    short          downtime_type;
    time_t         duration;
    time_t         end_time;
    time_t         entry_time;
    bool           fixed;
    unsigned int   host_id;
    unsigned int   instance_id;
    unsigned int   internal_id;
    unsigned int   service_id;
    time_t         start_time;
    unsigned int   triggered_by;
    bool           was_cancelled;
    bool           was_started;

  private:
    void           _internal_copy(downtime const& d);
  };
}

CCB_END()

#endif // !EVENTS_DOWNTIME_HH
