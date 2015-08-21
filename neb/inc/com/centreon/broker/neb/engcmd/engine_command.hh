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

#ifndef CCB_ENGCMD_ENGINE_COMMAND_HH
#  define CCB_ENGCMD_ENGINE_COMMAND_HH

#  include <string>
#  include <QMutex>
#  include "com/centreon/broker/extcmd/command_result.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                 neb {
namespace                 engcmd {
  /**
   *  @class engine_command engine_command.hh "com/centreon/broker/engcmd/engine_command.hh"
   *  @brief Engine command relay.
   *
   *  This stream relays command to centreon-engine's extcmd.so.
   */
  class                   engine_command : public io::stream {
  public:
                          engine_command(
                            std::string const& name,
                            std::string const& command_module_path);
                          ~engine_command();
    bool                  read(
                            misc::shared_ptr<io::data>& d,
                            time_t deadline = (time_t)-1);
    unsigned int          write(misc::shared_ptr<io::data> const& d);

  private:
                          engine_command(engine_command const& other);
    engine_command&       operator=(engine_command const& other);

    std::string           _name;
    std::string           _command_module_path;
    int                   (*_process_external_command)(char const*);

    void                  _load_command_engine_module();
    void                  _execute_command(std::string const cmd);
  };
}
}

CCB_END()

#endif // !CCB_ENGCMD_ENGINE_COMMAND_HH
