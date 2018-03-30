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
    _port(port),
    _size(0) {}

redisdb::~redisdb() {
  delete _socket;
}


/**
 *  Clear the redisdb object.
 */
void redisdb::clear() {
  _oss.str("");
  _content.clear();
  _size = 0;
}

redisdb& redisdb::operator<<(std::string const& str) {
  _oss << '$' << str.size() << "\r\n" << str << "\r\n";
  ++_size;
  return *this;
}

redisdb& redisdb::operator<<(int val) {
  std::ostringstream oss;
  oss << val;
  std::string l(oss.str());
  _oss << '$' << l.size() << "\r\n" << l << "\r\n";
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
    retval.append(_oss.str());
    return retval;
  }
  else {
    std::string retval(_oss.str());
    return retval;
  }
}

void redisdb::del() {
  push_command("$3\r\ndel\r\n");
}

void redisdb::hmset() {
  push_command("$5\r\nhmset\r\n");
}

QString& redisdb::flush() {
  if (_content.empty())
    throw (exceptions::msg()
      << "redis: Attempt to send empty data to "
      << _address << ":" << _port);

  _socket->connectToHost(_address.c_str(), _port);
  if (!_socket->waitForConnected())
    throw (exceptions::msg()
      << "redis: Couldn't connect to "
      << _address << ":" << _port
      << ": " << _socket->errorString().toStdString());

  if (_socket->write(_content.c_str()) != static_cast<qint64>(_content.size()))
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

void redisdb::push_command(std::string const& cmd) {
  _content.append(str(cmd));
  _oss.str("");
  _size = 0;
}

void redisdb::push(neb::host const& h) {
  // Values to push in redis:
  // * h:host_id
  //    - name
  std::ostringstream oss;
  oss << "h:" << h.host_id;
  std::string hst(oss.str());
  *this << oss.str()
    << "name" << h.host_name.toStdString();
  push_command("$4\r\nhset\r\n");
}

void redisdb::push(neb::host_status const& hs) {
  // Values to push in redis:
  // * h:host_id
  //    - state
  //    - enabled
  //    - acknowledged
  std::ostringstream oss;
  oss << "h:" << hs.host_id;
  std::string hst(oss.str());
  *this << oss.str()
    << "state" << hs.current_state
    << "enabled" << hs.enabled
    << "acknowledged" << hs.acknowledged;
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::service_status const& ss) {
  // Values to push in redis:
  // * s:host_id:service_id
  //     - state
  //     - enabled
  //     - acknowledged
  std::ostringstream oss;
  oss << "s:" << ss.host_id << ':' << ss.service_id;
  std::string svc(oss.str());
  *this << svc
    << "state" << ss.current_state
    << "enabled" << ss.enabled
    << "acknowledged" << ss.acknowledged;
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::service const& s) {
  // Values to push in redis:
  // * s:host_id:service_id
  //     - description
  std::ostringstream oss;
  oss << "s:" << s.host_id << ':' << s.service_id;
  std::string svc(oss.str());
  *this << svc
    << "description" << s.service_description.toStdString();
  push_command("$4\r\nhset\r\n");
}

std::string const&  redisdb::get_content() const {
  return _content;
}
