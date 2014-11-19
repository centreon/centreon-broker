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

CCB_BEGIN()

namespace             notification {
  // Forward declaration.
  class               process_manager;
  /**
   *  @class process process.hh "com/centreon/broker/notification/process.hh"
   *  @brief Represent a process being executed.
   */
  class               process {
  public:
                      process(int timeout = 0);

    unsigned int      get_timeout() const throw();
    bool              is_running() const;
    void              kill();

    bool              exec(
                        std::string const& program,
                        process_manager* manager = NULL);

    bool              is_timeout() const throw();

  private:
                      process(process const&);
    process&          operator=(process const&);

    unsigned int      _timeout;
    time_t            _start_time;
    std::auto_ptr<QProcess>
                      _process;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_PROCESS_HH
