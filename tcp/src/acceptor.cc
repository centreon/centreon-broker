/*
** Copyright 2011-2013,2015 Merethis
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
    _closed(false),
    _port(0),
    _read_timeout(-1),
    _write_timeout(-1) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  this->close();
}

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
 *  Close the acceptor.
 */
void acceptor::close() {
  _closed = true;
  QMutexLocker lock(&_mutex);
  if (_socket.get())
    _socket->close();
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
  while (!_closed && !_socket->has_pending_connections()) {
    bool timedout(false);
    bool ret(_socket->wait_for_new_connection(200, &timedout));
    while (!ret && timedout) {
      QWaitCondition cv;
      cv.wait(&_mutex, 100);
      timedout = false;
      ret = _socket->wait_for_new_connection(200, &timedout);
    }
    if (!ret) {
      if (_closed)
        throw (io::exceptions::shutdown(true, true)
               << "TCP: socket listening on port " << _port
               << " is closed");
      else
        throw (exceptions::msg() << "TCP: error while waiting client: "
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
