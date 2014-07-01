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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/local/connector.hh"
#include "com/centreon/broker/local/stream.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::local;

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
    _mutex(new QMutex),
    _timeout(-1) {}

/**
 *  @brief Copy constructor.
 *
 *  Only connection parameters are copied. The socket has to be opened
 *  after construction using open().
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c) : io::endpoint(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
connector::~connector() {
  this->close();
}

/**
 *  @brief Assignment operator.
 *
 *  The previous connection is first closed and then connection
 *  parameters are copied. The socket has to be opened after assignment
 *  using open().
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  if (this != &c) {
    this->close();
    io::endpoint::operator=(c);
    _internal_copy(c);
  }
  return (*this);
}

/**
 *  Clone the connector.
 *
 *  @return This object.
 */
io::endpoint* connector::clone() const {
  return (new connector(*this));
}

/**
 *  Close the socket.
 */
void connector::close() {
  QMutexLocker lock(&*_mutex);
  if (!_socket.isNull()) {
    _socket->close();
    _socket.clear();
  }
  return ;
}

/**
 *  Set the connection parameters.
 *
 *  @param[in] name Socket name.
 */
void connector::connect_to(QString const& name) {
  this->close();
  _name = name;
  return ;
}

/**
 *  Open the connection.
 *
 *  @return A new connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  // Close previous connection.
  this->close();

  // Lock mutex.
  QMutexLocker lock(&*_mutex);

  // Launch connection process.
  _socket = misc::shared_ptr<QLocalSocket>(new QLocalSocket);
  _socket->connectToServer(_name);

  // Wait for connection result.
  if (!_socket->waitForConnected()) {
    exceptions::msg e;
    e << "local: could not connect to '" << _name
      << "': " << _socket->errorString();
    _socket.clear();
    throw (e);
  }
  logging::info(logging::medium)
    << "local: successfully connected to '" << _name << "'";

  // Return stream.
  misc::shared_ptr<stream> s(new stream(_socket, _mutex));
  s->set_timeout(_timeout);
  return (s);
}

/**
 *  Open the connection.
 *
 *  @param[in] id Unused.
 *
 *  @return A new connection object.
 */
misc::shared_ptr<io::stream> connector::open(QString const& id) {
  (void)id;
  return (open());
}

/**
 *  Set connection timeout.
 *
 *  @param[in] msecs Timeout in ms.
 */
void connector::set_timeout(int msecs) {
  _timeout = msecs;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] c Object to copy.
 */
void connector::_internal_copy(connector const& c) {
  _name = c._name;
  _socket = c._socket;
  _timeout = c._timeout;
  return ;
}
