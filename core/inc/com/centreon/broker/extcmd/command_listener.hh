/*
** Copyright 2015 Merethis
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

#ifndef CCB_EXTCMD_COMMAND_LISTENER_HH
#  define CCB_EXTCMD_COMMAND_LISTENER_HH

#  include <map>
#  include <QMutex>
#  include "com/centreon/broker/extcmd/command_result.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 extcmd {
  /**
   *  @class command_listener command_listener.hh "com/centreon/broker/extcmd/command_listener.hh"
   *  @brief Command listener.
   *
   *  Listen to status of ongoing commands and provide them for some
   *  time. This is used by all command clients to fetch status of
   *  pending commands.
   */
  class                   command_listener : public io::stream {
  public:
                          command_listener();
                          ~command_listener();
    command_result        command_status(
                            unsigned int source_broker_id,
                            unsigned int command_id);
    bool                  read(
                            misc::shared_ptr<io::data>& d,
                            time_t deadline = (time_t)-1);
    unsigned int          write(misc::shared_ptr<io::data> const& d);

  private:
    struct                pending_command {
      time_t              invalid_time;
      command_result      result;
    };

                          command_listener(command_listener const& other);
    command_listener&     operator=(command_listener const& other);
    void                  _check_invalid();

    time_t                _next_invalid;
    std::map<std::pair<unsigned int, unsigned int>, pending_command>
                          _pending;
    QMutex                _pendingm;
    static int const      _request_timeout = 10;
    static int const      _result_timeout = 30;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_LISTENER_HH
