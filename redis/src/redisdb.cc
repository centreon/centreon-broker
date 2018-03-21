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
redisdb::redisdb()
  : _size(0),
    _socket(new QTcpSocket),
    _address("127.0.0.1"),
    _port(6379) {}

redisdb::redisdb(std::string const& address, unsigned short port)
  : _size(0),
    _socket(new QTcpSocket),
    _address(address),
    _port(port) {}

redisdb::~redisdb() {
  delete _socket;
}

void redisdb::set_parameters(std::string const& address, unsigned short port) {
  _address = address;
  _port = port;
}

/**
 *  Clear the redisdb object.
 */
void redisdb::clear() {
  _size = 0;
  _content = "";
}

redisdb& redisdb::operator<<(std::string const& str) {
  size_t first = 0;
  size_t idx(str.find_first_of(" \n\r"));
  while (idx != std::string::npos) {
    std::ostringstream oss;
    size_t size(idx - first);
    oss << size;
    _content += "$" + oss.str() + "\r\n"
              + str.substr(first, idx - first) + "\r\n";
    ++_size;
    first = str.find_last_of(" \n\r") + 1;
    idx = str.find_first_of(" \n\r", first);
  }
  if (first == 0) {
    std::ostringstream oss;
    oss << str.size();
    _content += "$" + oss.str() + "\r\n"
              + str + "\r\n";
  }
  else {
    std::string tmp(str.substr(first));
    std::ostringstream oss;
    oss << tmp.size();
    _content += "$" + oss.str() + "\r\n" + tmp + "\r\n";
  }
  ++_size;
  return *this;
}

redisdb& redisdb::operator<<(int val) {
  std::ostringstream oss;
  oss << val;
  _content += ":" + oss.str() + "\r\n";
  return *this;
}

std::string redisdb::str() {
  std::ostringstream oss;
  if (_size >= 2)
    oss << "*" << _size << "\r\n" << _content;
  else
    return _content;
  return oss.str();
}

void redisdb::flush() {
  _socket->connectToHost(_address.c_str(), _port);
  if (!_socket->waitForConnected())
    throw (exceptions::msg()
      << "redis: Couldn't connect to "
      << _address << ":" << _port
      << ": " << _socket->errorString().toStdString());

  std::string res(str());
  if (_socket->write(res.c_str(), res.size()) != static_cast<qint64>(res.size()))
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
  clear();
}
