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
#  include <QThread>
#  include <QMutex>
#  include <QSqlDatabase>
#  include <QWaitCondition>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

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
    void         rebuild_availabilities();

  private:
                 availability_thread(availability_thread const& other);
    bool         operator==(availability_thread const& other) const;

    bool         _build_availabilities(time_t since);
    bool         _compute_next_midnight(time_t& res);

    std::auto_ptr<QSqlDatabase>
                 _db;

    QMutex      _mutex;
    bool        _should_exit;
    bool        _should_rebuild_all;
    QWaitCondition
                _wait;
  };
}

CCB_END()

#endif // !CCB_BAM_AVAILABILITY_THREAD_HH
