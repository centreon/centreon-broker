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

#ifndef CCB_EXTCMD_COMMAND_CLIENT_HH
#  define CCB_EXTCMD_COMMAND_CLIENT_HH

#  include <memory>
#  include <QLocalSocket>
#  include <string>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             extcmd {
  // Forward declaration.
  class               command_listener;

  /**
   *  @class command_client command_client.hh "com/centreon/broker/extcmd/command_client.hh"
   *  @brief Command client.
   *
   *  This class represents a user connected to the command server.
   */
  class               command_client : public io::stream {
  public:
                      command_client(
                        int native_socket,
                        command_listener* listener);
                      ~command_client();
    bool              read(
                        misc::shared_ptr<io::data>& d,
                        time_t deadline = (time_t)-1);
    int               write(misc::shared_ptr<io::data> const& d);

  private:
                      command_client(command_client const& other);
    command_client&   operator=(command_client const& other);
    void              _initialize_socket();

    std::string       _buffer;
    command_listener* _listener;
    std::auto_ptr<QLocalSocket>
                      _socket;
    int               _socket_native;
  };
}

CCB_END()

#endif // !CCB_EXTCMD_COMMAND_CLIENT_HH
