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

#ifndef CCB_NOTIFICATION_PROCESS_MANAGER_HH
#  define CCB_NOTIFICATION_PROCESS_MANAGER_HH

#  include <memory>
#  include <list>
#  include <string>
#  include <QMutex>
#  include <QMutexLocker>
#  include <QThread>
#  include <QTimer>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/process.hh"

CCB_BEGIN()

namespace                     notification {
  /**
   *  @class process_manager process_manager.hh "com/centreon/broker/notification/processprocess_manager.hh"
   *  @brief Manage the processes.
   */
  class                       process_manager : public QThread {
  public:
    static process_manager&   instance();
    static void               release();

    virtual void              run();
    void                      create_process(
                                std::string const& command,
                                unsigned int timeout = 0);

    void                      add_timeout(unsigned int timeout);

  public slots:
    void                      process_finished();
    void                      process_timeouted();

  public:
    Q_OBJECT
  private:
    static process_manager*   _instance_ptr;
                              process_manager();
                              process_manager(process_manager const&);
    process_manager&          operator=(process_manager const&);

    QMutex                    _process_list_mutex;
    std::list<misc::shared_ptr<process> >
                              _process_list;
    std::list<misc::shared_ptr<QTimer> >
                              _timer_list;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_PROCESS_MANAGER_HH
