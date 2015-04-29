/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NEB_DOWNTIME_SCHEDULER_HH
#  define CCB_NEB_DOWNTIME_SCHEDULER_HH

#  include <map>
#  include <QThread>
#  include <QMutex>
#  include <QSemaphore>
#  include <QWaitCondition>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/neb/downtime.hh"

CCB_BEGIN()

namespace             neb {

  /**
   *  @class downtime_scheduler downtime_scheduler.hh "com/centreon/broker/notification/downtime_scheduler.hh"
   *  @brief The downtime scheduler.
   *
   *  Manage a thread that manages downtime end scheduling.
   */
  class        downtime_scheduler : public QThread {
  public:
               downtime_scheduler();

    void       start_and_wait();
    void       quit() throw ();
    void       add_downtime(
                 timestamp start_time,
                 timestamp end_time,
                 downtime const& dwn);

  protected:
    void       run();

  private:
    bool       _should_exit;
    QMutex     _general_mutex;
    QWaitCondition
               _general_condition;
    QSemaphore _started;

    std::multimap<timestamp, unsigned int>
               _downtime_starts;
    std::multimap<timestamp, unsigned int>
               _downtime_ends;
    std::map<unsigned int, downtime>
               _downtimes;

               downtime_scheduler(downtime_scheduler const& obj);
    downtime_scheduler&
               operator=(downtime_scheduler const& obj);

    static timestamp
               _get_first_timestamp(
                 std::multimap<timestamp, unsigned int> const& list);
    void       _process_downtimes();
    static void
               _start_downtime(downtime& dwn, io::stream* stream);
    static void
               _end_downtime(downtime& dwn, io::stream* stream);
  };
}

CCB_END()

#endif // !CCB_NEB_DOWNTIME_SCHEDULER_HH
