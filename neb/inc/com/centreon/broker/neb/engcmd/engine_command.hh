/*
** Copyright 2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_ENGCMD_ENGINE_COMMAND_HH
#  define CCB_ENGCMD_ENGINE_COMMAND_HH

#  include <string>
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
                            std::shared_ptr<io::data>& d,
                            time_t deadline = (time_t)-1);
    int                   write(std::shared_ptr<io::data> const& d);

  private:
                          engine_command(engine_command const& other);
    engine_command&       operator=(engine_command const& other);

    std::string           _name;
    void* _handle;
    std::string           _command_module_path;
    int                   (*_process_external_command)(char const*);

    void                  _load_command_engine_module();
    void                  _execute_command(std::string const cmd);
  };
}
}

CCB_END()

#endif // !CCB_ENGCMD_ENGINE_COMMAND_HH
