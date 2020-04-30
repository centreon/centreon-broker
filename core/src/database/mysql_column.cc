/*
** Copyright 2018-2020 Centreon
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

#include "com/centreon/broker/database/mysql_column.hh"

#include <cassert>
#include <cstring>
#include <iostream>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

mysql_column::mysql_column(int type, int row_count, int size)
    : _type(type),
      _row_count(row_count),
      _str_size(size),
      _vector(nullptr),
      _is_null(row_count),
      _error(row_count),
      _length(row_count) {
  if (type == MYSQL_TYPE_STRING && row_count && size) {
    char** vector(static_cast<char**>(malloc(_row_count * sizeof(char*))));
    for (int i = 0; i < _row_count; ++i) {
      vector[i] = static_cast<char*>(malloc(size));
      *vector[i] = 0;
    }
    _vector = vector;
  }
}

mysql_column::~mysql_column() {
  if (_vector) {
    if (_type == MYSQL_TYPE_STRING) {
      char** vector = static_cast<char**>(_vector);
      if (_str_size) {
        for (int i(0); i < _row_count; ++i) {
          free(vector[i]);
        }
      }
    }
    free(_vector);
  }
}

mysql_column::mysql_column(mysql_column&& other)
    : _type(other._type),
      _row_count(other._row_count),
      _str_size(other._str_size),
      _vector(other._vector),
      _is_null(other._is_null),
      _error(other._error),
      _length(other._length) {
  other._vector = nullptr;
}

mysql_column& mysql_column::operator=(mysql_column const& other) {
  if (this == &other)
    return *this;

  _type = other._type;
  _str_size = other._str_size;
  _row_count = other._row_count;
  _length = other._length;
  _error = other._error;
  _is_null = other._is_null;
  if (_vector)
    free(_vector);
  if (other._vector) {
    int size;
    switch (_type) {
      case MYSQL_TYPE_STRING:
        size = _row_count * sizeof(char*);
        break;
      case MYSQL_TYPE_FLOAT:
        size = _row_count * sizeof(float);
        break;
      case MYSQL_TYPE_LONG:
        size = _row_count * sizeof(int);
        break;
      case MYSQL_TYPE_TINY:
        size = _row_count * sizeof(char);
        break;
      case MYSQL_TYPE_DOUBLE:
        size = _row_count * sizeof(double);
        break;
      case MYSQL_TYPE_LONGLONG:
        size = _row_count * sizeof(long long);
        break;
      case MYSQL_TYPE_NULL:
        size = _row_count * sizeof(char*);
        break;
      default:
        assert(1 == 0);
    }
    if (_type == MYSQL_TYPE_STRING) {
      set_length(_str_size - 1);
      char** vector(static_cast<char**>(_vector));
      char** ovector(static_cast<char**>(other._vector));
      for (int i(0); i < _row_count; ++i)
        strncpy(vector[i], ovector[i], _str_size);
      _vector = vector;
    } else {
      _vector = malloc(size);
      memcpy(_vector, other._vector, size);
    }
  } else
    _vector = nullptr;
  return *this;
}

int mysql_column::get_type() const {
  return _type;
}

void* mysql_column::get_buffer() {
  return _vector;
}

void mysql_column::set_length(int len) {
  assert(_type == MYSQL_TYPE_STRING);
  _str_size = len + 1;
  if (!_vector)
    _vector = calloc(_row_count, sizeof(char*));

  char** vector = static_cast<char**>(_vector);
  for (int i(0); i < _row_count; ++i)
    vector[i] = static_cast<char*>(realloc(vector[i], _str_size));
}

void mysql_column::set_value(std::string const& str) {
  assert(_type == MYSQL_TYPE_STRING);
  size_t size = str.size();
  const char* content = str.c_str();
  std::string tmp;
  if (size > 65534) {
    tmp = str.substr(0, 65534);
    log_v2::sql()->warn(
        "mysql_column: Text column too short to contain a string of {} "
        "characters starting with '{}...'",
        size, str.substr(0, 30));
    size = tmp.size();
    content = tmp.c_str();
  }
  if (size >= _str_size)
    set_length(size);
  _length[0] = size;
  char** vector = static_cast<char**>(_vector);
  strncpy(vector[0], content, _length[0] + 1);
}

bool mysql_column::is_null() const {
  return _is_null[0];
}

my_bool* mysql_column::is_null_buffer() {
  return &_is_null[0];
}

my_bool* mysql_column::error_buffer() {
  return &_error[0];
}

unsigned long* mysql_column::length_buffer() {
  return &_length[0];
}

void mysql_column::set_type(int type) {
  _type = type;
  assert(_vector == nullptr);
  switch (type) {
    case MYSQL_TYPE_STRING:
      _vector = calloc(_row_count, sizeof(char*));
      break;
    case MYSQL_TYPE_FLOAT:
      _vector = calloc(_row_count, sizeof(float));
      break;
    case MYSQL_TYPE_LONG:
      _vector = calloc(_row_count, sizeof(int));
      break;
    case MYSQL_TYPE_TINY:
      _vector = calloc(_row_count, sizeof(char));
      break;
    case MYSQL_TYPE_DOUBLE:
      _vector = calloc(_row_count, sizeof(double));
      break;
    case MYSQL_TYPE_LONGLONG:
      _vector = calloc(_row_count, sizeof(long long));
      break;
    case MYSQL_TYPE_NULL:
      _vector = calloc(_row_count, sizeof(char*));
      break;
    default:
      assert(1 == 0);
  }
}
