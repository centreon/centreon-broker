/*
** Copyright 2011-2013 Centreon
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

#include <QMutexLocker>
#include <QWaitCondition>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/server_socket.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
acceptor::acceptor()
  : io::endpoint(true),
    _port(0),
    _read_timeout(-1),
    _write_timeout(-1) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {}

/**
 *  Add a child to this acceptor.
 *
 *  @param[in] child  Child name.
 */
void acceptor::add_child(std::string const& child) {
  QMutexLocker lock(&_childrenm);
  _children.push_back(child);
  return ;
}

/**
 *  Set the port on which the acceptor will listen.
 *
 *  @param[in] port Port on which the acceptor will listen.
 */
void acceptor::listen_on(unsigned short port) {
  _port = port;
  return ;
}

/**
 *  Start connection acception.
 *
 */
misc::shared_ptr<io::stream> acceptor::open() {
  // Listen on port.
  QMutexLocker lock(&_mutex);
  if (!_socket.get())
    _socket.reset(new server_socket(_port));

  // Wait for incoming connections.
  logging::debug(logging::medium) << "TCP: waiting for new connection";
  if (!_socket->has_pending_connections()) {
    bool timedout(false);
    _socket->wait_for_new_connection(1000, &timedout);
    if (!_socket->has_pending_connections()) {
      if (timedout)
        return (misc::shared_ptr<io::stream>());
      else
        throw (exceptions::msg()
               << "TCP: error while waiting client on port: " << _port
               << _socket->error_string());
    }
  }

  // Accept client.
  misc::shared_ptr<stream>
    incoming(_socket->next_pending_connection());
  if (incoming.isNull())
    throw (exceptions::msg() << "TCP: could not accept client: "
           << _socket->error_string());
  logging::info(logging::medium) << "TCP: new client connected";
  incoming->set_parent(this);
  incoming->set_read_timeout(_read_timeout);
  incoming->set_write_timeout(_write_timeout);
  return (incoming);
}

/**
 *  Remove child of this socket.
 *
 *  @param[in] child  Child to remove.
 */
void acceptor::remove_child(std::string const& child) {
  QMutexLocker lock(&_childrenm);
  for (std::list<std::string>::iterator
         it(_children.begin()),
         end(_children.end());
       it != end;
       ++it)
    if (*it == child) {
      _children.erase(it);
      break ;
    }
  return ;
}

/**
 *  @brief Set read timeout.
 *
 *  If child stream does not provide data frequently enough, it will
 *  time out after some configured seconds.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void acceptor::set_read_timeout(int secs) {
  _read_timeout = secs;
  return ;
}

/**
 *  Set write timeout on data.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void acceptor::set_write_timeout(int secs) {
  _write_timeout = secs;
  return ;
}

/**
 *  Get statistics about this TCP acceptor.
 *
 *  @param[out] tree Buffer in which statistics will be written.
 */
void acceptor::stats(io::properties& tree) {
  QMutexLocker children_lock(&_childrenm);
  std::ostringstream oss;
  oss << "peers=" << _children.size();
  for (std::list<std::string>::const_iterator
         it(_children.begin()),
         end(_children.end());
       it != end;
       ++it)
    oss << "\n  " << *it;
  io::property& p(tree["peers"]);
  p.set_perfdata(oss.str());
  p.set_graphable(false);
  return ;
}
