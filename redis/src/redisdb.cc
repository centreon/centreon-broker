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

#include <iostream>
#include <QHostAddress>
#include <QTcpSocket>
#include <QStringList>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/redis/redisdb.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

static std::string build_key(std::string const& head, unsigned int id) {
  /* We count one digit in radix 10 for 3 bits. Yes we are large... */
  char buf[sizeof(unsigned int) * 8 / 3 + 2 + head.size()];
  snprintf(buf, sizeof(buf), "%s:%d", head.c_str(), id);
  return std::string(buf);
}

static std::string build_key(
                     std::string const& head,
                     unsigned int id1,
                     unsigned id2) {
  /* We count one digit in radix 10 for 3 bits. Yes we are large... */
  char buf[2 * sizeof(unsigned int) * 8 / 3 + 3 + head.size()];
  snprintf(buf, sizeof(buf), "%s:%d:%d", head.c_str(), id1, id2);
  return std::string(buf);
}

/**
 *  Default constructor.
 */
redisdb::redisdb(
           std::string const& address,
           unsigned short port,
           std::string const& password,
           unsigned int queries_per_transaction)
  : _socket(new QTcpSocket),
    _address(address),
    _port(port),
    _password(password),
    _queries_per_transaction(queries_per_transaction),
    _size(0) {
  _connect();
  _check_redis_server();
}

redisdb::~redisdb() {
  send(true);
  delete _socket;
}

void redisdb::_check_redis_server() {
  int row1 = info("Server");
  int row2 = module("list");
  send(true);
  QVariantList res(parse(_result));
  QByteArray res1(res[row1].toByteArray());
  QVariantList lst(res[row2].toList());

  if (!res1.contains("redis_version"))
    throw (exceptions::msg()
      << "redis: Unable to grab informations from Redis server");
  _result.clear();
  bool module_found(false);
  bool version_ok(false);

  // Checking if redistabular module is present
  if (lst.size() >= 1) {
    for (int i(0); i < lst.size(); ++i) {
      QVariantList module(lst[i].toList());
      for (int j(0); j < module.size(); j += 2) {
        QString const& key(module[j].toString());
        QVariant const& value(module[j + 1]);

        if (key == "name" && value.toString() == "tabular")
          module_found = true;
        else if (key == "ver" && value.toInt() >= 1)
          version_ok = true;
      }
      if (module_found)
        break;
    }
  }
  if (!module_found || !version_ok) {
    throw (exceptions::msg()
      << "redis: The redistabular module is not installed or its version"
      << " is too old ( < 1) on the redis server '"
      << _address << ":" << _port << "'");
  }
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
  char buf[sizeof(unsigned long int) * 8 / 3 + 4];
  snprintf(buf, sizeof(buf), "$%lu\r\n", str.size());
  _content.append(buf).append(str).append("\r\n");
  return *this;
}

redisdb& redisdb::operator<<(int val) {
  char buf[sizeof(int) * 8 / 3 + 1];
  snprintf(buf, sizeof(buf), "%d", val);
  char buffer[sizeof(unsigned long int) * 8 / 3 + 4];
  snprintf(buffer, sizeof(buffer), "$%lu\r\n", strlen(buf));
  _content.append(buffer).append(buf).append("\r\n");
  return *this;
}

std::string redisdb::str(std::string const& cmd) {
  return _content;
}

void redisdb::push_array(size_t size) {
  char buf[sizeof(unsigned int) * 8 / 3 + 4];
  snprintf(buf, sizeof(buf), "*%lu\r\n", size);
  _content.append(buf);
}

QByteArray& redisdb::send(bool force = false) {
  if (_size >= _queries_per_transaction || (force && _size > 0)) {
    if (_socket->state() != QTcpSocket::ConnectedState)
      _connect();

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
  }
  else
    logging::debug(logging::low)
      << "redis: data not pushed (" << _size << " queries on "
      << _queries_per_transaction << ")";
  return _result;
}

// Redis Write commands
void redisdb::push(instance_broadcast const& ib) {
  logging::debug(logging::low)
    << "redis: push instance_broadcast";
  // Here, there are some cleanup to do...
  // FIXME DBR
}

void redisdb::push(neb::host_group_member const& hgm) {
  // Values to push in redis:
  // * hg:host_id : it is a bitfield of hostgroup_id
  logging::info(logging::high)
    << "redis: push host_group_member "
    << "(host_id: " << hgm.host_id << ", group_id: " << hgm.group_id << ")";

  sadd(build_key("hg", hgm.group_id), hgm.host_id);
  send();
  _check_validity();
}

void redisdb::push(neb::host const& h) {
  logging::info(logging::high)
    << "redis: push host "
    << "(host_id: " << h.host_id << ")";

  std::string hst(build_key("h", h.host_id));

  sadd("hosts", hst);
  hmset(hst, 8);
  *this << "name" << h.host_name.toStdString()
        << "alias" << h.alias.toStdString()
        << "address" << h.address.toStdString()
        << "action_url" << h.action_url.toStdString()
        << "notes" << h.notes.toStdString()
        << "notes_url" << h.notes_url.toStdString()
        << "poller_id" << h.poller_id
        << "icon_image" << h.icon_image.toStdString();

  setbit(build_key("p", h.poller_id), h.host_id, 1);
  send();
  _check_validity();
}

void redisdb::push(neb::host_status const& hs) {
  logging::info(logging::high)
    << "redis: push host_status "
    << "(host_id: " << hs.host_id << ")";

  std::string hst(build_key("h", hs.host_id));

  hmset(hst, 7);
  *this << "current_state" << hs.current_state
        << "enabled" << hs.enabled
        << "scheduled_downtime_depth" << hs.downtime_depth
        << "plugin_output" << hs.output.toStdString()
        << "active_checks" << hs.active_checks_enabled
        << "passive_checks" << hs.passive_checks_enabled
        << "acknowledged" << hs.acknowledged;

  send();
  _check_validity();
}

void redisdb::push(neb::instance const& inst) {
  logging::info(logging::high)
    << "redis: push instance "
    << "(poller_id: " << inst.poller_id << ")";
  // Values to push in redis:
  // * i:poller_id
  //    - name
  std::string ist(build_key("i", inst.poller_id));
  set(ist, inst.name.toStdString());
  send();
  _check_validity();
}

void redisdb::push(neb::service_group_member const& sgm) {
  logging::info(logging::high)
    << "redis: push service_group_member "
    << "(host_id: " << sgm.host_id << ", service_id: " << sgm.service_id << ")";
  // Values to push in redis:
  // sg:group_id { s:host_id:service_id } it is a set of service keys.

  std::string sgstr(build_key("sg", sgm.group_id));
  std::string svc(build_key("s", sgm.host_id, sgm.service_id));
  sadd(sgstr, svc);
  _check_validity();
}

void redisdb::push(neb::service const& s) {
  logging::info(logging::high)
    << "redis: push service "
    << "(host_id: " << s.host_id << ", service_id: " << s.service_id << ")";

  std::string hst(build_key("h", s.host_id));
  int row1(hmget(hst, 4, "name", "poller_id", "host_groups", "acl_groups"));
  std::string svc(build_key("s", s.host_id, s.service_id));
  QVariantList res(parse(send(true)));
  QVariantList lst(res[row1].toList());

  sadd(build_key("services", s.host_id), svc);
  sadd("services", svc);

  hmset(svc, 16);
  *this << "service_description" << s.service_description.toStdString()
        << "host_id" << s.host_id
        << "service_id" << s.service_id
        << "notify" << s.notifications_enabled
        << "action_url" << s.action_url.toStdString()
        << "notes" << s.notes.toStdString()
        << "last_state_change" << s.last_state_change
        << "notes_url" << s.notes_url.toStdString()
        << "event_handler_enabled" << s.event_handler_enabled
        << "flapping" << s.is_flapping
        << "icon_image" << s.icon_image.toStdString()
        << "display_name" << s.display_name.toStdString()
        << "host_name" << lst[0].toByteArray().constData()
        << "poller_id" << lst[1].toInt()
        << "host_groups" << lst[2].toByteArray().constData()
        << "acl_groups" << lst[3].toByteArray().constData();

  send();
  _check_validity();
}

void redisdb::push(neb::service_status const& ss) {
  logging::info(logging::high)
    << "redis: push service_status "
    << "(host_id: " << ss.host_id << ", service_id: " << ss.service_id << ")";

  std::string svc(build_key("s", ss.host_id, ss.service_id));

  hmset(svc, 15);
  *this << "current_state" << ss.current_state
        << "state_type" << ss.state_type
        << "last_check" << ss.last_check
        << "next_check" << ss.next_check
        << "max_check_attempts" << ss.max_check_attempts
        << "last_state_change" << ss.last_state_change
        << "last_hard_state_change" << ss.last_hard_state_change
        << "enabled" << ss.enabled
        << "scheduled_downtime_depth" << ss.downtime_depth
        << "plugin_output" << ss.output.toStdString()
        << "current_check_attempt" << ss.current_check_attempt
        << "flap_detection" << ss.flap_detection_enabled
        << "active_checks" << ss.active_checks_enabled
        << "passive_checks" << ss.passive_checks_enabled
        << "acknowledged" << ss.acknowledged;
  send();

  _check_validity();
}

int redisdb::del(std::string const& key) {
  push_array(2);
  *this << "DEL" << key;
  return _size++;
}

int redisdb::unlink(std::string const& key) {
  push_array(2);
  *this << "UNLINK" << key;
  return _size++;
}

int redisdb::hset(std::string const& key, std::string const& member, std::string const& value) {
  logging::debug(logging::low)
    << "redis: hset " << key << ": " << member << " => " << value;
  push_array(4);
  *this << "HSET" << key << member << value;
  return _size++;
}

int redisdb::set(std::string const& key, std::string const& value) {
  logging::debug(logging::low)
    << "redis: set " << key << " => " << value;
  push_array(3);
  *this << "SET" << key << value;
  return _size++;
}

int redisdb::set(std::string const& key, int value) {
  logging::debug(logging::low)
    << "redis: set " << key << " => " << value;
  push_array(3);
  *this << "SET" << key << value;
  return _size++;
}

int redisdb::incr(std::string const& key) {
  logging::debug(logging::low)
    << "redis: incr " << key;
  push_array(2);
  *this << "INCR" << key;
  return _size++;
}

int redisdb::hmset(std::string const& key, int count) {
  logging::debug(logging::low)
    << "redis: hmset " << key;
  push_array((count << 1) + 2);
  *this << "HMSET" << key;
  return _size++;
}

int redisdb::sadd(std::string const& key, int item) {
  push_array(3);
  *this << "SADD" << key << item;
  return _size++;
}

int redisdb::sadd(std::string const& key, std::string const& item) {
  push_array(3);
  *this << "SADD" << key << item;
  return _size++;
}

int redisdb::setbit(std::string const& key, long int row, int value) {
  logging::debug(logging::low)
    << "redis: setbit";
  push_array(4);
  *this << "SETBIT" << key << row << value;
  return _size++;
}

int redisdb::srem(std::string const& key, std::string const& item) {
  push_array(3);
  *this << "SREM" << key << item;
  return _size++;
}

// Redis Read commands
int redisdb::get(std::string const& key) {
  logging::debug(logging::low)
    << "redis: get " << key;
  push_array(2);
  *this << "GET" << key;
  return _size++;
}

int redisdb::getbit(std::string const& key, long long index) {
  logging::debug(logging::low)
    << "redis: getbit " << key;
  push_array(3);
  *this << "GETBIT" << key << index;
  return _size++;
}

int redisdb::hget(std::string const& key, std::string const& item) {
  logging::debug(logging::low)
    << "redis: hget " << key << " => " << item;
  push_array(3);
  *this << "HGET" << key << item;
  return _size++;
}

int redisdb::hgetall(std::string const& key) {
  logging::debug(logging::low)
    << "redis: hgetall";
  push_array(2);
  *this << "HGETALL" << key;
  return _size++;
}

int redisdb::hmget(std::string const& key, int count, ...) {
  logging::debug(logging::low)
    << "redis: hmget";
  push_array(count + 2);
  va_list list;
  va_start(list, count);
  *this << "HMGET" << key;
  for (int i = 0; i < count; ++i)
    *this << va_arg(list, char const*);
  va_end(list);
  return _size++;
}

int redisdb::info(std::string const& arg) {
  push_array(2);
  *this << "INFO" << arg;
  return _size++;
}

int redisdb::keys(std::string const& pattern) {
  logging::debug(logging::low)
    << "redis: keys " << pattern;
  push_array(2);
  *this << "KEYS" << pattern;
  return _size++;
}

/**
 *  hmset binding to the redis hmset function
 *
 * @param key The hash key
 * @param count The items count to affect
 */
int redisdb::module(std::string const& arg) {
  push_array(2);
  *this << "MODULE" << arg;
  return _size++;
}

int redisdb::smembers(std::string const& key) {
  push_array(2);
  *this << "SMEMBERS" << key;
  return _size++;
}

int redisdb::sismember(std::string const& key, std::string const& item) {
  logging::debug(logging::low)
    << "redis: hgetall";
  push_array(3);
  *this << "SISMEMBER" << key << item;
  return _size++;
}

//QByteArray& redisdb::push(neb::custom_variable const& cv) {
//  // We only work with CRITICALITY_LEVEL and CRITICALITY_ID for now.
//  // For a host:
//  // * h:host_id
//  //     - criticality_id or criticality_level
//  //
//  // For a service:
//  // * s:host_id:service_id
//  //     - criticality_id or criticality_level
//  //
//  logging::info(logging::high)
//    << "redis: push custom var "
//    << " host_id " << cv.host_id
//    << " service_id " << cv.service_id
//    << " name " << cv.name.toStdString()
//    << " value " << cv.value.toStdString();
//
//  std::string tag;
//  if (cv.name == "CRITICALITY_ID")
//    tag = "criticality_id";
//  else if (cv.name == "CRITICALITY_LEVEL")
//    tag = "criticality_level";
//  else {
//    _result.clear();
//    return _result;
//  }
//
//  std::ostringstream oss;
//  if (cv.service_id) {
//    oss << "s:" << cv.host_id << ':' << cv.service_id;
//    *this << "services" << oss.str();
//  }
//  else {
//    oss << "h:" << cv.host_id;
//    *this << "hosts" << oss.str();
//  }
//
//  *this << 1 << "REPLACE" << "PARTIAL" << "FIELDS"
//        << tag << cv.value.toStdString();
//  push_command("$6\r\nFT.ADD\r\n");
//  return send();
//}

void redisdb::_check_validity() const {
  char const* tmp(strstr(_result.constData(), "-ERR"));
  if (tmp) {
    throw (exceptions::msg()
      << "redis: Error while sending data to Redis server - "
      << tmp);
  }
}

std::string const&  redisdb::get_content() const {
  return _content;
}

void redisdb::_connect() {
  _socket->connectToHost(_address.c_str(), _port);
  if (!_socket->waitForConnected())
    throw (exceptions::msg()
      << "redis: Couldn't connect to "
      << _address << ":" << _port
      << ": " << _socket->errorString().toStdString());

  if (!_password.empty()) {
    std::ostringstream oss;
    oss << "*2\r\n$4\r\nauth\r\n"
      << '$' << _password.size() << "\r\n" << _password << "\r\n";

    std::string query(oss.str());
    if (_socket->write(query.c_str()) != static_cast<qint64>(query.size()))
      throw (exceptions::msg()
        << "redis: Couldn't authenticate to "
      << _address << ":" << _port
      << ": " << _socket->errorString().toStdString());

    while (_socket->bytesToWrite()) {
      if (!_socket->waitForBytesWritten())
        throw (exceptions::msg()
          << "redis: Couldn't send authentication to "
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
    if (_socket->readAll() != "+OK\r\n")
      throw (exceptions::msg()
        << "redis: Couldn't authenticate to "
        << _socket->peerAddress().toString().toStdString()
        << ":" << _socket->peerPort()
        << ": bad password");
  }
}

QVariantList redisdb::parse(QByteArray const& array) {
  logging::debug(logging::low)
    << "redis: parse";
  QByteArray::const_iterator it(array.begin());
  QVariantList retval;
  do {
    retval.append(_parse(array, it));
  } while (it != array.end());
  return retval;
}

QVariant redisdb::_parse(QByteArray const& arr,
                         QByteArray::const_iterator& it) {
  logging::debug(logging::low)
    << "redis: _parse";

  switch (*it) {
    case '$':
      return _parse_str(arr, it);
    case '*':
      return _parse_array(arr, it);
    case ':':
      return _parse_int(arr, it);
    case '+':
    case '-':
      return _parse_simple_str(arr, it);
    default:
      throw (exceptions::msg()
        << "redis: Unable to parse the string '"
        << arr.constData() << "' that is nor a string, nor an array");
  }
  return QVariant();
}

QVariant redisdb::_parse_str(QByteArray const& arr, QByteArray::const_iterator& it) {
  logging::debug(logging::low)
    << "redis: _parse_str";
  size_t len(0);
  if (*it != '$')
    throw (exceptions::msg()
      << "redis: Unable to parse '" << arr.constData() << "' as a string");
  ++it;
  if (strncmp(it, "-1", 2) == 0) {
    it += 4;
    return QVariant();
  }
  while (*it != '\r') {
    len = len * 10 + (*it - '0');
    ++it;
  }

  /* Now it points at '\r' */
  it += 2;
  QVariant retval(QVariant(QByteArray(it, len)));
  it += len + 2;
  return retval;
}

QVariant redisdb::_parse_simple_str(QByteArray const& arr, QByteArray::const_iterator& it) {
  logging::debug(logging::low)
    << "redis: _parse_simple_str";
  if (*it != '+' && *it != '-')
    throw (exceptions::msg()
      << "redis: Unable to parse '" << arr.constData() << "' as a string");
  QByteArray::const_iterator end(it);
  while (*end != '\r')
    ++end;

  QVariant retval(QByteArray(it, end - it));
  it = end + 2;
  return retval;
}

QVariant redisdb::_parse_int(QByteArray const& arr, QByteArray::const_iterator& it) {
  logging::debug(logging::low)
    << "redis: _parse_int";
  int value(0);
  if (*it != ':')
    throw (exceptions::msg()
      << "redis: Unable to parse '" << arr.constData() << "' as an integer");
  ++it;
  while (*it != '\r') {
    value = value * 10 + *it - '0';
    ++it;
  }

  /* Now it points at '\r' */
  it += 2;
  return QVariant(value);
}

QVariant redisdb::_parse_array(QByteArray const& arr, QByteArray::const_iterator& it) {
  logging::debug(logging::low)
    << "redis: _parse_array";
  size_t array_size(0);
  if (*it != '*')
    throw (exceptions::msg()
      << "redis: Unable to parse '" << arr.constData() << "' as an array");
  ++it;
  while (*it != '\r') {
    array_size = array_size * 10 + (*it - '0');
    ++it;
  }

  /* Now it points at '\r' */
  it += 2;
  QVariantList retval;
  while (array_size > 0) {
    QVariant ret(_parse(arr, it));
    retval.append(ret);
    --array_size;
  }
  return QVariant(retval);
}

std::string const& redisdb::get_address() const {
  return _address;
}

unsigned short const redisdb::get_port() const {
  return _port;
}
