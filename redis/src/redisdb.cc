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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/redis/redisdb.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::redis;

/**
 *  Default constructor.
 */
redisdb::redisdb(
           std::string const& address,
           unsigned short port,
           std::string const& password)
  : _socket(new QTcpSocket),
    _address(address),
    _port(port),
    _password(password),
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
  if (!_socket->state() != QTcpSocket::ConnectedState)
    _connect();

  if (_content.empty())
    throw (exceptions::msg()
      << "redis: Attempt to send empty data to "
      << _address << ":" << _port);

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
  return _result;
}

void redisdb::push_command(std::string const& cmd) {
  _content.append(str(cmd));
  _oss.str("");
  _size = 0;
}

void redisdb::push(neb::custom_variable const& cv) {
  // We only work with CRITICALITY_LEVEL and CRITICALITY_ID for now.
  // For a host:
  // * h:host_id
  //     - criticality_id or criticality_level
  //
  // For a service:
  // * s:host_id:service_id
  //     - criticality_id or criticality_level
  //
  logging::info(logging::high)
    << "redis: push custom var "
    << " host_id " << cv.host_id
    << " service_id " << cv.service_id
    << " name " << cv.name.toStdString()
    << " value " << cv.value.toStdString();

  std::string tag;
  if (cv.name == "CRITICALITY_ID")
    tag = "criticality_id";
  else if (cv.name == "CRITICALITY_LEVEL")
    tag = "criticality_level";
  else
    return ;

  std::ostringstream oss;
  if (cv.service_id) {
    oss << "s:" << cv.host_id
      << ':' << cv.service_id;
  }
  else
    oss << "h:" << cv.host_id;

  *this << oss.str()
        << tag << cv.value.toStdString();
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::host_group_member const& hgm) {
  // Values to push in redis:
  // * hgm:host_id : it is a set of hostgroup_id's
  std::ostringstream oss;
  oss << "hgm:" << hgm.host_id;
  *this << oss.str()
    << hgm.group_id;
  push_command("$4\r\nsadd\r\n");
}

void redisdb::push(neb::host const& h) {
  // Values to push in redis:
  // * h:host_id
  //    - name
  //    - alias
  //    - address
  //    - action_url
  //    - notes
  //    - notes_url
  //    - poller_id
  std::ostringstream oss;
  oss << "h:" << h.host_id;
  *this << oss.str()
    << "name" << h.host_name.toStdString()
    << "alias" << h.alias.toStdString()
    << "address" << h.address.toStdString()
    << "action_url" << h.action_url.toStdString()
    << "notes" << h.notes.toStdString()
    << "notes_url" << h.notes_url.toStdString()
    << "icon_image" << h.icon_image.toStdString()
    << "poller_id" << h.poller_id;
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::host_status const& hs) {
  // Values to push in redis:
  // * h:host_id
  //    - state
  //    - enabled
  //    - acknowledged
  std::ostringstream oss;
  oss << "h:" << hs.host_id;
  *this << oss.str()
    << "state" << hs.current_state
    << "enabled" << hs.enabled
    << "scheduled_downtime_depth" << hs.downtime_depth
    << "plugin_output" << hs.output.toStdString()
    << "active_checks" << hs.active_checks_enabled
    << "passive_checks" << hs.passive_checks_enabled
    << "acknowledged" << hs.acknowledged;
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::instance const& inst) {
  // Values to push in redis:
  // * i:poller_id
  //    - name
  std::ostringstream oss;
  oss << "i:" << inst.poller_id;
  std::string ist(oss.str());
  *this << oss.str()
    << "name" << inst.name.toStdString();
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::service_group_member const& sgm) {
  // Values to push in redis:
  // * sgm:host_id:service_id : it is a set of servicegroup_id's
  std::ostringstream oss;
  oss << "sgm:" << sgm.host_id << ':' << sgm.service_id;
  *this << oss.str()
    << sgm.group_id;
  push_command("$4\r\nsadd\r\n");
}

void redisdb::push(neb::service_status const& ss) {
  // Values to push in redis:
  // * s:host_id:service_id
  //     - state
  //     - last_state_change
  //     - enabled
  //     - acknowledged
  std::ostringstream oss;
  oss << "s:" << ss.host_id << ':' << ss.service_id;
  std::string svc(oss.str());
  logging::info(logging::high)
    << "redis: hmset " << svc
    << " state " << ss.current_state
    << " state_type " << ss.state_type
    << " last_check " << ss.last_check
    << " next_check " << ss.last_check
    << " max_check_attempts " << ss.max_check_attempts
    << " last_state_change " << ss.last_state_change
    << " last_hard_state_change " << ss.last_hard_state_change
    << " enabled " << ss.enabled
    << " scheduled_downtime_depth " << ss.downtime_depth
    << " plugin_output " << ss.output.toStdString()
    << " current_check_attempt " << ss.current_check_attempt
    << " flap_detection " << ss.flap_detection_enabled
    << " active_checks " << ss.active_checks_enabled
    << " passive_checks " << ss.passive_checks_enabled
    << " acknowledged " << ss.acknowledged;

  *this << svc
    << "state" << ss.current_state
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
  push_command("$5\r\nhmset\r\n");
}

void redisdb::push(neb::service const& s) {
  // Values to push in redis:
  // * s:host_id:service_id
  //     - description
  //     - notes
  //     - notes_url
  //     - display_name
  std::ostringstream oss;
  oss << "s:" << s.host_id << ':' << s.service_id;
  std::string svc(oss.str());
  *this << svc
    << "description" << s.service_description.toStdString()
    << "notify" << s.notifications_enabled
    << "action_url" << s.action_url.toStdString()
    << "notes" << s.notes.toStdString()
    << "notes_url" << s.notes_url.toStdString()
    << "event_handler_enabled" << s.event_handler_enabled
    << "flapping" << s.is_flapping
    << "icon_image" << s.icon_image.toStdString()
    << "display_name" << s.display_name.toStdString();
  push_command("$5\r\nhmset\r\n");
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
