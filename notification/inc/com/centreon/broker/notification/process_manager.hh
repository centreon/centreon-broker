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
#  include <set>
#  include <string>
#  include <QMutex>
#  include <QMutexLocker>
#  include <QThread>
#  include <QTimer>
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/notification/process.hh"

namespace                          com {
  namespace                        centreon {
    namespace                      broker {
      namespace                    notification {
        /**
         *  @class process_manager process_manager.hh "com/centreon/broker/notification/processprocess_manager.hh"
         *  @brief Manage the processes.
         */
        class                      process_manager : public QObject {
          Q_OBJECT

        public:
          static process_manager&  instance();
          static void              release();

          void                     create_process(
                                     std::string const& command,
                                     unsigned int timeout = 0);

          QThread&                 get_thread();

        public slots:
          void                     process_finished(process&);
          void                     process_timeouted(process&);

        private:
          static process_manager*  _instance_ptr;
                                   process_manager();
                                   process_manager(process_manager const&);
          process_manager&         operator=(process_manager const&);

          QMutex                   _process_list_mutex;
          std::set<process*>       _process_list;

          std::auto_ptr<QThread>   _thread;
        };
      }
    }
  }
}

#endif // !CCB_NOTIFICATION_PROCESS_MANAGER_HH
