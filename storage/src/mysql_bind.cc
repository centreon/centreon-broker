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
#include "com/centreon/broker/storage/mysql_bind.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

my_bool mysql_bind::_true = 1;

mysql_bind::mysql_bind(int size) {
  _bind.resize(size);
  _buffer.resize(size);
  _length.resize(size);
}

mysql_bind::~mysql_bind() {}

void mysql_bind::set_size(int size) {
  _bind.resize(size);
  _buffer.resize(size);
  _length.resize(size);
}

void mysql_bind::set_string(int range, std::string const& value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_STRING;
  _buffer[range] = value;
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _bind[range].is_null = 0;
  _length[range] = value.size();
  _bind[range].length = &_length[range];
}

void mysql_bind::set_tiny(int range, char value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_TINY;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
//  for (int i = 0; i < sizeof(value); ++i)
//    _buffer[range][i] = tmp[i];
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_int(int range, int value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
//  for (int i = 0; i < sizeof(value); ++i)
//    _buffer[range][i] = tmp[i];
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
}

void mysql_bind::set_uint(int range, unsigned int value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
//  for (int i = 0; i < sizeof(value); ++i)
//    _buffer[range][i] = tmp[i];
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  _bind[range].is_unsigned = true;
}

void mysql_bind::set_float(int range, float value) {
  if (range >= _bind.size())
    _bind.resize(range + 1);
  memset(&_bind[range], 0, sizeof(MYSQL_BIND));
  _bind[range].buffer_type = MYSQL_TYPE_FLOAT;
  char* tmp = reinterpret_cast<char*>(&value);
  _buffer[range].resize(sizeof(value));
  memcpy(&_buffer[range][0], tmp, sizeof(value));
//  for (int i = 0; i < sizeof(value); ++i)
//    _buffer[range][i] = tmp[i];
  _bind[range].buffer = const_cast<char*>(_buffer[range].c_str());
  if (isnan(value))
    _bind[range].is_null = &_true;
}

MYSQL_BIND const* mysql_bind::get_bind() const {
  return &_bind[0];
}
