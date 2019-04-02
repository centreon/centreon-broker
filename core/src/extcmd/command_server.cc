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

#include <QLocalSocket>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_client.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/extcmd/command_server.hh"
#include "com/centreon/broker/extcmd/plaintext_command_parser.hh"
#include "com/centreon/broker/extcmd/json_command_parser.hh"
#include "com/centreon/broker/extcmd/server_socket.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/processing/feeder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**
 *  Constructor.
 *
 *  @param[in] prot         The protocol used by this server.
 *  @param[in] socket_file  Socket file.
 *  @param[in] cache        Endpoint persistent cache.
 */
command_server::command_server(
                  protocol prot,
                  std::string const& socket_file,
                  std::shared_ptr<persistent_cache> cache)
  : io::endpoint(true),
    _listener_thread(NULL),
    _protocol(prot),
    _socket_file(socket_file) {
  (void)cache;
}

/**
 *  Destructor.
 */
command_server::~command_server() {
  if (_listener_thread) {
    _listener_thread->exit();
    _listener_thread->wait();
    delete _listener_thread;
  }
}

/**
 *  Wait for a new connection to local socket.
 *
 *  @return New client object if available, null pointer otherwise.
 */
misc::shared_ptr<io::stream> command_server::open() {
  // Initialization.
  if (!_socket.get()) {
    // Listen on socket.
    ::remove(_socket_file.c_str());
    _socket.reset(new server_socket(_socket_file));

    // Create command listener.
    _listener = new command_listener;
    // Create command parser.
    if (_protocol == json)
      _parser = new json_command_parser(*_listener);
    else
      _parser = new plaintext_command_parser(*_listener);
    // Create listener thread.
    uset<unsigned int> write_filters;
    write_filters.insert(command_request::static_type());
    write_filters.insert(command_result::static_type());
    _listener_thread = new processing::feeder(
                                         "(external commands)",
                                         _listener,
                                         uset<unsigned int>(),
                                         write_filters);
    _listener_thread->start();
  }

  // Wait for incoming connections.
  logging::debug(logging::medium)
    << "command: waiting for new connection";
  if (!_socket->has_pending_connections()) {
    bool timedout(false);
    _socket->wait_for_new_connection(1000, &timedout);
    if (!_socket->has_pending_connections()) {
      if (timedout)
        return (misc::shared_ptr<io::stream>());
      else
        throw (exceptions::msg()
               << "command: error while waiting on client on file '"
               << _socket_file << "': " << _socket->error_string());
    }
  }

  // Accept new client.
  int incoming(_socket->next_pending_connection());
  if (incoming < 0)
    throw (exceptions::msg() << "command: could not accept client: "
           << _socket->error_string());
  logging::info(logging::medium) << "command: new client connected";
  misc::shared_ptr<io::stream>
    new_client(new command_client(incoming, *_parser.data()));
  return (new_client);
}
