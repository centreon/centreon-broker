/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_AVAILABILITY_THREAD_HH
#  define CCB_BAM_AVAILABILITY_THREAD_HH

#  include <string>
#  include <memory>
#  include <map>
#  include <QThread>
#  include <QMutex>
#  include <QSqlDatabase>
#  include <QWaitCondition>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/bam/time/timeperiod.hh"
#  include "com/centreon/broker/bam/availability_builder.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class availability_thread availability_thread.hh "com/centreon/broker/bam/availability_thread.hh"
   *  @brief Availability thread
   *
   */
  class          availability_thread : public QThread {
  public:
                 availability_thread(QString const& db_type,
                                     QString const& db_host,
                                     unsigned short db_port,
                                     QString const& db_user,
                                     QString const& db_password,
                                     QString const& db_name);
                 ~availability_thread();
    virtual void run();
    void         terminate();

    void         clear_timeperiods();
    void         register_timeperiod(time::timeperiod::ptr);
    void         rebuild_availabilities();

  private:
                 availability_thread(availability_thread const& other);
    bool         operator==(availability_thread const& other) const;

    bool         _build_availabilities(time_t day_start, time_t day_end);
    bool         _compute_next_midnight(time_t& res);
    static void  _write_availability(QSqlQuery& q,
                                     availability_builder const& builder,
                                     unsigned int ba_id,
                                     time_t day_start,
                                     unsigned int timeperiod_id);

    std::auto_ptr<QSqlDatabase>
                 _db;
    std::map<unsigned int,
              time::timeperiod::ptr>
                _timeperiods;

    QMutex      _mutex;
    bool        _should_exit;
    bool        _should_rebuild_all;
    QWaitCondition
                _wait;
  };
}

CCB_END()

#endif // !CCB_BAM_AVAILABILITY_THREAD_HH
