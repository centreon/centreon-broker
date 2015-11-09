/*
** Copyright 2011-2015 Centreon
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

#include <memory>
#include <sstream>
#include <QMutexLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/stream.hh"

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
connector::connector()
  : io::endpoint(false),
    _port(0),
    _read_timeout(-1),
    _write_timeout(-1),
    _child(NULL) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other) : io::endpoint(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Set connection parameters.
 *
 *  @param[in] host  Host to connect to.
 *  @param[in] port  Port to connect to.
 */
void connector::connect_to(QString const& host, unsigned short port) {
  _host = host;
  _port = port;
  return ;
}

/**
 *  Clone this connector.
 *
 *  @return  A clone of this connector.
 */
io::endpoint* connector::clone() const {
  return (new connector(*this));
}

/**
 *  Close this connector.
 */
void connector::close() {
  QMutexLocker lock(&_mutex);
  if (_child) {
    // Notify the child that it should close.
    _child->close();
  }
}

/**
 *  Connect to the remote host.
 */
misc::shared_ptr<io::stream> connector::open() {
  QMutexLocker lock(&_mutex);
  // Launch connection process.
  logging::info(logging::medium) << "TCP: connecting to "
    << _host << ":" << _port;
  std::string connection_name;
  {
    std::ostringstream oss;
    oss << _host.toStdString() << ":" << _port;
    connection_name = oss.str();
  }
  std::auto_ptr<QTcpSocket> sock(new QTcpSocket);
  sock->connectToHost(_host, _port);

  // Wait for connection result.
  if (!sock->waitForConnected())
    throw (exceptions::msg() << "TCP: could not connect to "
           << connection_name << ": " << sock->errorString());
  logging::info(logging::medium)
    << "TCP: successfully connected to " << connection_name;

  // Return stream.
  misc::shared_ptr<stream> s(new stream(sock.get(), connection_name));
  sock.release();
  s->set_read_timeout(_read_timeout);
  s->set_write_timeout(_write_timeout);
  s->set_parent(this);
  return (s);
}

/**
 *  Connect to the remote host.
 *
 *  @param[in] id  Unused.
 */
misc::shared_ptr<io::stream> connector::open(QString const& id) {
  (void)id;
  return (open());
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_read_timeout(int secs) {
  _read_timeout = secs;
  return ;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void connector::set_write_timeout(int secs) {
  _write_timeout = secs;
  return ;
}

/**
 *  Associate a child stream with the connector.
 *  @param[in] child  The child stream to associate.
 */
void connector::add_child(tcp::stream& child) {
  if (_child)
    _child->close();
  _child = &child;
}

/**
 *  Remove a child stream associated with the connector.
 *  @param[in] child  The child stream to remove.
 */
void connector::remove_child(tcp::stream& child) {
  QMutexLocker lock(&_mutex);
  if (_child == &child)
    _child = NULL;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void connector::_internal_copy(connector const& other) {
  _host = other._host;
  _port = other._port;
  _read_timeout = other._read_timeout;
  _write_timeout = other._write_timeout;
  return ;
}
