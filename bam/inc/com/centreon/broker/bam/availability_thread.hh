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
#  include <set>
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
    void         rebuild_availabilities(QString const& bas_to_rebuild);

  private:
                 availability_thread(availability_thread const& other);
    availability_thread&
                  operator=(availability_thread const& other) const;

    void         _delete_all_availabilities();
    void         _build_availabilities(time_t midnight);
    void         _build_daily_availabilities(QSqlQuery &q,
                                             time_t day_start,
                                             time_t day_end);
    static void  _write_availability(QSqlQuery& q,
                                     availability_builder const& builder,
                                     unsigned int ba_id,
                                     time_t day_start,
                                     unsigned int timeperiod_id);

    void         _compute_next_midnight(time_t& res);
    void         _compute_start_of_day(time_t when,
                                       time_t& res);

    void        _open_database();
    void        _close_database();

    QString     _db_type;
    QString     _db_host;
    unsigned short
                _db_port;
    QString     _db_user;
    QString     _db_password;
    QString     _db_name;
    std::auto_ptr<QSqlDatabase>
                 _db;
    std::map<unsigned int,
              time::timeperiod::ptr>
                _timeperiods;

    QMutex      _mutex;
    bool        _should_exit;
    bool        _should_rebuild_all;
    QString     _bas_to_rebuild;
    QWaitCondition
                _wait;
  };
}

CCB_END()

#endif // !CCB_BAM_AVAILABILITY_THREAD_HH
