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

#include <cassert>
#include <iostream>
#include <cfloat>
#include <cstring>
#include <cmath>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/mysql_bind.hh"

using namespace com::centreon::broker;

mysql_bind::mysql_bind() {
  set_size(0);
}

/**
 *  Constructor
 *
 * @param size Number of columns in the bind
 * @param length Size reserved for each column's buffer. By default, this value
 *               is 0. So, no reservation is made.
 */
mysql_bind::mysql_bind(int size, int length) {
  _bind.resize(size);
  _buffer.resize(size);
  _length.resize(size);
  _is_null.resize(size);
  _error.resize(size);
  for (int i(0); i < size; ++i) {
    _bind[i].buffer_type = MYSQL_TYPE_STRING;
    _buffer[i].str = new std::string();
    _buffer[i].str->reserve(length);
    _bind[i].buffer = const_cast<char*>(_buffer[i].str->c_str());
    _bind[i].buffer_length = length;
    _bind[i].length = &_length[i];
    _bind[i].is_null = &_is_null[i];
    _bind[i].error = &_error[i];
  }
}

mysql_bind::~mysql_bind() {
  for (int i(0); i < _buffer.size(); ++i) {
    if (_bind[i].buffer_type == MYSQL_TYPE_STRING)
      delete _buffer[i].str;
  }
}

void mysql_bind::set_size(int size, int length) {
  _bind.resize(size);
  _buffer.resize(size);
  _length.resize(size);
  _is_null.resize(size);
  _error.resize(size);
  for (int i(0); i < size; ++i) {
    if (_bind[i].buffer_type == MYSQL_TYPE_STRING) {
      if (length) {
        if (length > _buffer[i].str->capacity()) {
          _buffer[i].str->reserve(length);
          _bind[i].buffer_length = length;
        }
      }
      else
        _bind[i].buffer_length = _buffer[i].str->capacity();

      _bind[i].buffer = const_cast<char*>(_buffer[i].str->c_str());
    }
    else {
      _bind[i].buffer = &_buffer[i];
      _bind[i].buffer_length = 0;
    }

    _bind[i].length = &_length[i];
    _bind[i].is_null = &_is_null[i];
    _bind[i].error = &_error[i];
  }
}

std::string const& mysql_bind::value_as_str(int range) const {
  assert(_bind[range].buffer_type == MYSQL_TYPE_STRING);
  return *_buffer[range].str;
}

void mysql_bind::set_value_as_str(int range, std::string const& value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    *_buffer[range].str = value;
  else {
    _buffer[range].str = new std::string(value);
    _bind[range].buffer_type = MYSQL_TYPE_STRING;
  }
  _bind[range].buffer = const_cast<char*>(_buffer[range].str->c_str());
  _bind[range].buffer_length = _buffer[range].str->capacity();
  _length[range] = value.size();
  _bind[range].length = &_length[range];
  _is_null[range] = 0;
  _error[range] = 0;
}

void mysql_bind::set_value_as_tiny(int range, char value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  _bind[range].buffer_type = MYSQL_TYPE_TINY;
  _buffer[range].c = value;
  _bind[range].buffer_length = 0;
  _is_null[range] = 0;
  _length[range] = 0;
  _error[range] = 0;
}

bool mysql_bind::value_as_bool(int range) const {
  bool retval;
  switch (_bind[range].buffer_type) {
    case MYSQL_TYPE_TINY:
      retval =  _buffer[range].c;
      break;
    case MYSQL_TYPE_STRING:
      retval =  *_buffer[range].str->c_str()
        ? strtol(_buffer[range].str->c_str(), 0, 10) : 0;
      break;
    default:
      assert(1 == 0); // This line should ne be reached
  }
  return retval;
}

void mysql_bind::set_value_as_bool(int range, bool value) {
  set_value_as_tiny(range, value ? 1 : 0);
}

void mysql_bind::set_value_as_i32(int range, int value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  _buffer[range].i32 = value;
  _bind[range].buffer_length = 0;
  _bind[range].is_unsigned = false;
  _is_null[range] = 0;
  _length[range] = 0;
  _error[range] = 0;
}

int mysql_bind::value_as_i32(int range) const {
  int retval;
  switch (_bind[range].buffer_type) {
    case MYSQL_TYPE_TINY:
      retval =  _buffer[range].c;
      break;
    case MYSQL_TYPE_LONG:
      retval =  _buffer[range].i32;
      break;
    case MYSQL_TYPE_STRING:
      retval =  _buffer[range].str->c_str()
        ? strtol(_buffer[range].str->c_str(), 0, 10) : 0;
      break;
    default:
      assert(1 == 0); // This line should not be reached
  }
  return retval;
}

void mysql_bind::set_value_as_u32(int range, unsigned int value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  _bind[range].buffer_type = MYSQL_TYPE_LONG;
  _buffer[range].i32 = value;
  _bind[range].buffer_length = 0;
  _bind[range].is_unsigned = true;
  _is_null[range] = 0;
  _length[range] = 0;
  _error[range] = 0;
}

unsigned int mysql_bind::value_as_u32(int range) const {
  unsigned int retval;
  switch (_bind[range].buffer_type) {
    case MYSQL_TYPE_TINY:
      retval =  _buffer[range].c;
      break;
    case MYSQL_TYPE_LONG:
      retval = _buffer[range].i32;
      break;
    case MYSQL_TYPE_STRING:
      retval =  _buffer[range].str->c_str()
        ? strtoul(_buffer[range].str->c_str(), 0, 10) : 0;
      break;
    default:
      assert(1 == 0); // This line should not be reached
  }
  return retval;
}

void mysql_bind::set_value_as_u64(int range, unsigned long long value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  _bind[range].buffer_type = MYSQL_TYPE_LONGLONG;
  _buffer[range].i64 = value;
  _bind[range].buffer_length = 0;
  _bind[range].is_unsigned = true;
  _is_null[range] = 0;
  _length[range] = 0;
  _error[range] = 0;
}

unsigned long long mysql_bind::value_as_u64(int range) const {
  unsigned long long retval;
  switch (_bind[range].buffer_type) {
    case MYSQL_TYPE_TINY:
      retval =  _buffer[range].c;
      break;
    case MYSQL_TYPE_LONGLONG:
      retval = _buffer[range].i64;
      break;
    case MYSQL_TYPE_STRING:
      retval =  _buffer[range].str->c_str()
        ? strtoull(_buffer[range].str->c_str(), 0, 10) : 0;
      break;
    default:
      assert(1 == 0); // This line should not be reached
  }
  return retval;
}

void mysql_bind::set_value_as_f32(int range, float value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  if (std::isnan(value) || std::isinf(value))
    set_value_as_null(range);
  else {
    _bind[range].buffer_type = MYSQL_TYPE_FLOAT;
    _buffer[range].f32 = value;
    _bind[range].buffer_length = 0;
    _is_null[range] = 0;
    _length[range] = 0;
    _error[range] = 0;
  }
}

float mysql_bind::value_as_f32(int range) const {
  float retval;
  switch (_bind[range].buffer_type) {
    case MYSQL_TYPE_STRING:
      retval =  _buffer[range].str->c_str()
        ? atof(_buffer[range].str->c_str()) : 0;
      break;
    case MYSQL_TYPE_FLOAT:
      retval = _buffer[range].f32;
      break;
    default:
      assert(1 == 0); // This line should not be reached
  }
  return retval;
}

void mysql_bind::set_value_as_f64(int range, double value) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  if (std::isnan(value) || std::isinf(value))
    set_value_as_null(range);
  else {
    _bind[range].buffer_type = MYSQL_TYPE_DOUBLE;
    _buffer[range].f64 = value;
    _bind[range].buffer_length = 0;
    _is_null[range] = 0;
    _length[range] = 0;
    _error[range] = 0;
  }
}

double mysql_bind::value_as_f64(int range) const {
  double retval;
  switch (_bind[range].buffer_type) {
    case MYSQL_TYPE_STRING:
      retval =  _buffer[range].str->c_str()
        ? atof(_buffer[range].str->c_str()) : 0;
      break;
    case MYSQL_TYPE_DOUBLE:
      retval = _buffer[range].f64;
      break;
    default:
      assert(1 == 0); // This line should not be reached
  }
  return retval;
}

void mysql_bind::set_value_as_null(int range) {
  if (range >= _bind.size())
    set_size(range + 1);
  if (_bind[range].buffer_type == MYSQL_TYPE_STRING)
    delete _buffer[range].str;
  _bind[range].buffer_type = MYSQL_TYPE_NULL;
  _is_null[range] = 0;
  _length[range] = 0;
  _error[range] = 0;
}

bool mysql_bind::value_is_null(int range) const {
  if (_is_null[range] || _bind[range].buffer_type == MYSQL_TYPE_NULL)
    return true;
  else
    return false;
}

void mysql_bind::debug() {
  std::cout << "DEBUG BIND " << this << std::endl;
  int size(_bind.size());
  for (int i(0); i < size; ++i) {
    switch (_bind[i].buffer_type) {
      case MYSQL_TYPE_LONGLONG:
        {
          std::cout << "LONGLONG : L:" << *_bind[i].length
            << " : " << "BL: " << _bind[i].buffer_length
            << " : " << _buffer[i].i64 << std::endl;
        }
        break;
      case MYSQL_TYPE_LONG:
        {
          std::cout << "LONG : L:" << *_bind[i].length
            << " : " << "BL: " << _bind[i].buffer_length
            << " : " << _buffer[i].i32 << std::endl;
        }
        break;
      case MYSQL_TYPE_TINY:
        {
          std::cout << "TINY : L:" << *_bind[i].length
            << " : " << "BL: " << _bind[i].buffer_length
            << " : " << _buffer[i].c << std::endl;
        }
        break;
      case MYSQL_TYPE_NULL:
        std::cout << "NULL : L:" << *_bind[i].length << " : "
            << " : " << "BL: " << _bind[i].buffer_length;
        break;
      case MYSQL_TYPE_ENUM:
        std::cout << "ENUM : L:" << *_bind[i].length << " : "
            << " : " << "BL: " << _buffer[i].str->size() << "/" << _bind[i].buffer_length
            << " : " << "BL: " << _buffer[i].str->size();
        break;
      case MYSQL_TYPE_STRING:
        std::cout << "STRING : L:" << *_bind[i].length << " : "
            << " : " << "BL: " << _buffer[i].str->size() << "/" << _bind[i].buffer_length
            << " : " << "BL: " << _buffer[i].str->size();
        break;
      case MYSQL_TYPE_DOUBLE:
        {
          std::cout << "DOUBLE : L:" << *_bind[i].length
            << " : " << "BL: " << _bind[i].buffer_length
            << " : " << _buffer[i].f64 << std::endl;
        }
        break;
      case MYSQL_TYPE_FLOAT:
        {
          std::cout << "FLOAT : L:" << *_bind[i].length
            << " : " << "BL: " << _bind[i].buffer_length
            << " : " << _buffer[i].f32 << std::endl;
        }
        break;
      default:
        std::cout << _bind[i].buffer_type << " : L:" << *_bind[i].length
          << " : " << "BL: " << _bind[i].buffer_length
          << " : " << _buffer[i].str
          << " : " << "TYPE NOT MANAGED...\n";
        assert(1 == 0);   // Should not arrive...
        break;
    }
    std::cout << std::endl;
  }
}

bool mysql_bind::is_empty() const {
  return _is_empty;
}

MYSQL_BIND const* mysql_bind::get_bind() const {
  return &_bind[0];
}

MYSQL_BIND* mysql_bind::get_bind() {
  return &_bind[0];
}

int mysql_bind::get_size() const {
  return _bind.size();
}

/**
 *  At the moment, the bind only carries one row. So this number is 0 or 1.
 *  And that does not implies that the result is so small.
 *
 * @return 1 or 0.
 */
int mysql_bind::get_rows_count() const {
  return _is_empty ? 0 : 1;
}

void mysql_bind::set_empty(bool empty) {
  _is_empty = empty;
}
