/*
** Copyright 2011-2012,2015 Merethis
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

#include <QWaitCondition>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/local/acceptor.hh"
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
acceptor::acceptor() : endpoint(true) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy connection parameters but does not put socket in listen mode.
 *  You'll still need to call open() after the construction.
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : io::endpoint(a) {
  _name = a._name;
}

/**
 *  Destructor.
 */
acceptor::~acceptor() {
  this->close();
}

/**
 *  @brief Assignment operator.
 *
 *  Destroy the old listener and copy connection parameters. However the
 *  internal socket won't be in listen mode until you call open().
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  if (this != &a) {
    this->close();
    io::endpoint::operator=(a);
    _name = a._name;
  }
  return (*this);
}

/**
 *  Clone the socket.
 *
 *  @return This object.
 */
io::endpoint* acceptor::clone() const {
  return (new acceptor(*this));
}

/**
 *  Close the socket.
 */
void acceptor::close() {
  QMutexLocker lock(&_mutex);
  if (_socket.get()) {
    _socket->close();
    _socket.reset();
  }
  return ;
}

/**
 *  Set the local socket name.
 *
 *  @param[in] name Local socket name.
 */
void acceptor::listen_on(QString const& name) {
  this->close();
  _name = name;
  return ;
}

/**
 *  Open the acceptor.
 *
 *  @return A new stream.
 */
misc::shared_ptr<io::stream> acceptor::open() {
  // Listen.
  QMutexLocker lock(&_mutex);
  if (!_socket.get()) {
    _socket.reset(new QLocalServer);
    if (!_socket->listen(_name)) {
      exceptions::msg e;
      e << "local: could not listen on socket '" << _name
        << "': " << _socket->errorString();
      _socket.reset();
      throw (e);
    }
  }

  // Wait for incoming connections.
  logging::debug(logging::medium)
    << "local: waiting for new connection";
  bool timedout(false);
  bool ret(_socket->waitForNewConnection(200, &timedout));
  while (!ret && timedout) {
    QWaitCondition cv;
    cv.wait(&_mutex, 10);
    timedout = false;
    ret = _socket.get()
      && _socket->waitForNewConnection(200, &timedout);
  }
  if (!ret)
    throw (exceptions::msg() << "local: error while waiting client: "
           << (!_socket.get()
               ? "socket was deleted"
               : _socket->errorString()));

  // Accept client.
  misc::shared_ptr<QLocalSocket>
    incoming(_socket->nextPendingConnection());
  if (incoming.isNull())
    throw (exceptions::msg() << "local: could not accept client: "
             << _socket->errorString());
  logging::info(logging::medium) << "local: new client connected";

  // Return object.
  return (misc::shared_ptr<io::stream>(new stream(incoming)));
}

/**
 *  Open the acceptor.
 *
 *  @param[in] id     Unused.
 *
 *  @return A new stream.
 */
misc::shared_ptr<io::stream> acceptor::open(QString const& id) {
  (void)id;
  return (open());
}
