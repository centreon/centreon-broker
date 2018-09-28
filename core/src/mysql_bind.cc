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

#include <cfloat>
#include <cstring>
#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mysql_bind.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/mysql.hh"

using namespace com::centreon::broker;

mysql_bind::mysql_bind() {
  set_size(0);
}

mysql_bind::mysql_bind(int size) {
  set_size(size);
}

mysql_bind::mysql_bind(mysql_bind const& other)
 : _buffer(other._buffer),
   _length(other._length) {
  int size(_length.size());
  _bind.resize(size);
  for (int i(0); i < size; ++i) {
    memset(&_bind[i], 0, sizeof(MYSQL_BIND));
    _bind[i].buffer_type = other._bind[i].buffer_type;
    _bind[i].buffer = const_cast<char*>(_buffer[i].c_str());
    //if (_length[i])
    _bind[i].length = &_length[i];
  }
}

mysql_bind::~mysql_bind() {}

void mysql_bind::set_size(int size) {
  _bind.resize(size);
  _buffer.resize(size);
  _length.resize(size);
  for (int i(0); i < size; ++i) {
    _bind[i].buffer = const_cast<char*>(_buffer[i].c_str());
    //if (_length[i])
    _bind[i].length = &_length[i];
  }
}

void mysql_bind::set_value_as_str(int range, std::string const& value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_STRING;
  _buffer[range] = value;
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _length[range] = value.size();
  _bind[range].length = &_length[range];
}

void mysql_bind::set_value_as_tiny(int range, char value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_TINY;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_value_as_bool(int range, bool value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_TINY;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_value_as_i32(int range, int value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_value_as_u32(int range, unsigned int value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _bind[range].is_unsigned = true;
}

void mysql_bind::set_value_as_u64(int range, unsigned long long value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _bind[range].is_unsigned = true;
}

void mysql_bind::set_value_as_f32(int range, float value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  if (isnan(value) || isinf(value))
    _bind[range].buffer_type = MYSQL_TYPE_NULL;
  else {
    _bind[range].buffer_type = MYSQL_TYPE_FLOAT;
    char* tmp = reinterpret_cast<char*>(&value);
    _buffer[range].resize(sizeof(value));
    memcpy(&_buffer[range][0], tmp, sizeof(value));
    _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  }
}

void mysql_bind::set_value_as_f64(int range, double value) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  if (isnan(value) || isinf(value))
    _bind[range].buffer_type = MYSQL_TYPE_NULL;
  else {
    _bind[range].buffer_type = MYSQL_TYPE_DOUBLE;
    char* tmp = reinterpret_cast<char*>(&value);
    _buffer[range].resize(sizeof(value));
    memcpy(&_buffer[range][0], tmp, sizeof(value));
    _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  }
}

void mysql_bind::set_value_as_null(int range) {
  if (range >= _bind.size())
    set_size(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_NULL;
}

MYSQL_BIND const* mysql_bind::get_bind() const {
  return &_bind[0];
}

int mysql_bind::get_size() const {
  return _bind.size();
}
