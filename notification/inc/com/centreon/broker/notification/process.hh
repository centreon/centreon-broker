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

#ifndef CCB_NOTIFICATION_PROCESS_HH
#  define CCB_NOTIFICATION_PROCESS_HH

#  include <memory>
#  include <string>
#  include <QProcess>
#  include "com/centreon/broker/namespace.hh"
#  include <QObject>

namespace                          com {
  namespace                        centreon {
    namespace                      broker {
      namespace                    notification {
      // Forward declaration.
      class               process_manager;
      /**
   *  @class process process.hh "com/centreon/broker/notification/process.hh"
   *  @brief Represent a process being executed.
   */
      class               process : public QObject {
        Q_OBJECT

      public:
                          process(int timeout = 0);

        unsigned int      get_timeout() const throw();
        bool              is_running() const;
        void              kill();

        bool              exec(
                            std::string const& program,
                            process_manager* manager = NULL);

        bool              is_timeout() const throw();

        bool              get_error(
                            int& exit_code,
                            std::string& error_output);

      public slots:
        void              start(QString const& command_line);
        void              error();
        void              finished();
        void              timeouted();

      signals:
        void              finished(process&);
        void              timeouted(process&);

      private:
                          process(process const&);
        process&          operator=(process const&);

        unsigned int      _timeout;
        time_t            _start_time;
        std::auto_ptr<QProcess>
                          _process;

        bool              _in_error;
        QProcess::ProcessError
                          _error;
        QProcess::ExitStatus
                          _status;
        int               _exit_code;
        std::string       _error_output;
      };
      }
    }
  }
}

#endif // !CCB_NOTIFICATION_PROCESS_HH
