/*
** Copyright 2018 Centreon
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

#include <QHostAddress>
#include <QTcpSocket>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/redis/redisdb.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

/**
 *  Default constructor.
 */
redisdb::redisdb(
           std::string const& address,
           unsigned short port,
           std::string const& user,
           std::string const& password)
  : _socket(new QTcpSocket),
    _address(address),
    _port(port) {}

redisdb::~redisdb() {
  delete _socket;
}


/**
 *  Clear the redisdb object.
 */
void redisdb::clear() {
  _content.str("");
  _size = 0;
}

redisdb& redisdb::operator<<(std::string const& str) {
  _content << '$' << str.size() << "\r\n" << str << "\r\n";
  ++_size;
  return *this;
}

redisdb& redisdb::operator<<(int val) {
  std::ostringstream oss;
  oss << val;
  std::string l(oss.str());
  _content << '$' << l.size() << "\r\n" << l << "\r\n";
  ++_size;
  return *this;
}

std::string redisdb::str(std::string const& cmd) {
  unsigned int size;
  if (cmd.empty())
    size = _size;
  else
    size = _size + 1;
  if (size >= 2) {
    std::ostringstream oss;
    oss << size;
    std::string l(oss.str());
    std::string retval("*");
    retval.append(l).append("\r\n");
    if (!cmd.empty())
      retval.append(cmd);
    retval.append(_content.str());
    return retval;
  }
  else {
    std::string retval(_content.str());
    return retval;
  }
}

QString& redisdb::mset() {
  return send_command("$4\r\nmset\r\n");
}

QString& redisdb::send_command(std::string const& cmd) {
  _socket->connectToHost(_address.c_str(), _port);
  if (!_socket->waitForConnected())
    throw (exceptions::msg()
      << "redis: Couldn't connect to "
      << _address << ":" << _port
      << ": " << _socket->errorString().toStdString());

  std::string res(str(cmd));
  if (_socket->write(res.c_str()) != static_cast<qint64>(res.size()))
    throw (exceptions::msg()
      << "redis: Couldn't write content to "
      << _address << ":" << _port
      << ": " << _socket->errorString().toStdString());

  while (_socket->bytesToWrite()) {
    if (!_socket->waitForBytesWritten())
      throw (exceptions::msg()
        << "redis: Couldn't send data to "
        << _socket->peerAddress().toString().toStdString()
        << ":" << _socket->peerPort()
        << ": " << _socket->errorString().toStdString());
  }

  if (!_socket->waitForReadyRead()) {
    throw (exceptions::msg()
      << "redis: Couldn't read data from "
      << _socket->peerAddress().toString().toStdString()
      << ":" << _socket->peerPort()
      << ": " << _socket->errorString().toStdString());
  }
  _result.clear();
  _result.append(_socket->readAll());
  clear();
  _socket->close();
  return _result;
}

redisdb& redisdb::operator<<(neb::host_status const& hs) {
  // Values to push in redis:
  // * host_id:current_state
  // * host_id:check_type
  // * host_id:next_check
  // * host_id:state_type
  std::ostringstream oss;
  oss << hs.host_id;
  std::string key;
  key.reserve(64);
  key.append(oss.str()).append(":current_state");
  *this << key << hs.current_state;
  key.clear();
  key.append(oss.str()).append(":check_type");
  *this << key << hs.check_type;
  key.clear();
  key.append(oss.str()).append(":next_check");
  *this << key << hs.next_check;
  key.clear();
  key.append(oss.str()).append(":state_type");
  *this << key << hs.state_type;
  return *this;
}

redisdb& redisdb::operator<<(neb::service_status const& ss) {
  // Values to push in redis:
  // * host_id:service_id:current_state
  // * host_id:service_id:check_type
  // * host_id:service_id:next_check
  // * host_id:service_id:state_type
  std::ostringstream oss;
  oss << ss.host_id << ':' << ss.service_id;
  std::string key;
  key.reserve(64);
  key.append(oss.str()).append(":current_state");
  *this << key << ss.current_state;
  key.clear();
  key.append(oss.str()).append(":check_type");
  *this << key << ss.check_type;
  key.clear();
  key.append(oss.str()).append(":next_check");
  *this << key << ss.next_check;
  key.clear();
  key.append(oss.str()).append(":state_type");
  *this << key << ss.state_type;
  return *this;
}
