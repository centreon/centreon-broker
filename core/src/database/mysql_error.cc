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
#include "com/centreon/broker/database/mysql_error.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

mysql_error::mysql_error() : _message(""), _active(false), _fatal(false) {}

mysql_error::mysql_error(char const* message, bool fatal)
    : _message(message), _active(true), _fatal(fatal) {}

mysql_error::mysql_error(mysql_error&& other) {
  _message = move(other._message);
  _active = other._active;
  other._active = false;
  _fatal = other._fatal;
  other._fatal = false;
}

mysql_error& mysql_error::operator=(mysql_error const& other) {
  if (this != &other) {
    _message = other._message;
    _fatal = other._fatal;
    _active = other._active;
  }
  return *this;
}

std::string mysql_error::get_message() const {
  return _message;
}

bool mysql_error::is_fatal() const {
  return _fatal;
}

void mysql_error::clear() {
  _active = false;
}

bool mysql_error::is_active() const {
  return _active;
}
