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

#include "com/centreon/broker/database/mysql_bind.hh"
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <iostream>
#include "com/centreon/broker/database/mysql_column.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mysql.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

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
mysql_bind::mysql_bind(int size, int length)
    : _bind(size), _column(size), _typed(size) {
  if (length) {
    for (int i(0); i < size; ++i) {
      _bind[i].buffer_type = MYSQL_TYPE_STRING;
      _column[i] = mysql_column(MYSQL_TYPE_STRING, 1, length);
      _bind[i].buffer = *static_cast<char**>(_column[i].get_buffer());
      _bind[i].is_null = _column[i].is_null_buffer();
      _bind[i].length = _column[i].length_buffer();
      _bind[i].buffer_length = length;
      _bind[i].error = _column[i].error_buffer();
    }
  }
}

mysql_bind::~mysql_bind() {}

void mysql_bind::set_size(int size, int length) {
  _bind.resize(size);
  _column.resize(size);
  for (int i(0); i < size; ++i) {
    if (length && _column[i].get_type() == MYSQL_TYPE_STRING)
      _column[i].set_length(length);

    _bind[i].buffer = _column[i].get_buffer();
  }
}

bool mysql_bind::_prepared(int range) const {
  return _typed[range];
}

void mysql_bind::_prepare_type(int range, enum enum_field_types type) {
  _typed[range] = true;
  _bind[range].buffer_type = type;
  _column[range].set_type(type);
}

char* mysql_bind::value_as_str(int range) {
  assert(_bind[range].buffer_type == MYSQL_TYPE_STRING);
  return static_cast<char*>(_bind[range].buffer);
}

void mysql_bind::set_value_as_str(int range, std::string const& value) {
  assert(static_cast<uint32_t>(range) < _bind.size());
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_STRING);
  _column[range].set_value(value);
  _bind[range].buffer = *static_cast<char**>(_column[range].get_buffer());
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

void mysql_bind::set_value_as_tiny(int range, char value) {
  assert(static_cast<uint32_t>(range) < _bind.size());
  // if (range >= _bind.size())
  //  set_size(range + 1);
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_TINY);
  //_bind[range].buffer_type = MYSQL_TYPE_TINY;
  _column[range].set_value(value);
  _bind[range].buffer = _column[range].get_buffer();
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

bool mysql_bind::value_as_bool(int range) const {
  assert(_bind[range].buffer_type == MYSQL_TYPE_TINY);
  return *static_cast<char*>(_bind[range].buffer);
}

void mysql_bind::set_value_as_bool(int range, bool value) {
  set_value_as_tiny(range, value ? 1 : 0);
}

void mysql_bind::set_value_as_i32(int range, int value) {
  assert(static_cast<uint32_t>(range) < _bind.size());
  // if (range >= _bind.size())
  //  set_size(range + 1);
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_LONG);
  //_bind[range].buffer_type = MYSQL_TYPE_LONG;
  _bind[range].is_unsigned = false;
  _column[range].set_value(value);
  _bind[range].buffer = _column[range].get_buffer();
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

int mysql_bind::value_as_i32(int range) const {
  if (_bind[range].buffer_type == MYSQL_TYPE_LONG)
    return *static_cast<int*>(_bind[range].buffer);
  else if (_bind[range].buffer_type == MYSQL_TYPE_STRING) {
    int retval(strtol(static_cast<char*>(_bind[range].buffer), nullptr, 10));
    return retval;
  } else
    assert("This field is not an int" == nullptr);
}

void mysql_bind::set_value_as_u32(int range, uint32_t value) {
  assert(static_cast<uint32_t>(range) < _bind.size());
  // if (range >= _bind.size())
  //  set_size(range + 1);
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_LONG);
  //_bind[range].buffer_type = MYSQL_TYPE_LONG;
  _bind[range].is_unsigned = true;
  _column[range].set_value(value);
  _bind[range].buffer = _column[range].get_buffer();
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

uint32_t mysql_bind::value_as_u32(int range) const {
  if (_bind[range].buffer_type == MYSQL_TYPE_LONG)
    return *static_cast<uint32_t*>(_bind[range].buffer);
  else if (_bind[range].buffer_type == MYSQL_TYPE_STRING) {
    uint32_t retval(
        strtoul(static_cast<char*>(_bind[range].buffer), nullptr, 10));
    return retval;
  } else
    assert("This field is not an uint32_t" == nullptr);
}

void mysql_bind::set_value_as_u64(int range, unsigned long long value) {
  assert(static_cast<uint32_t>(range) < _bind.size());
  // if (range >= _bind.size())
  //  set_size(range + 1);
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_LONGLONG);
  //_bind[range].buffer_type = MYSQL_TYPE_LONGLONG;
  _bind[range].is_unsigned = true;
  _column[range].set_value(value);
  _bind[range].buffer = _column[range].get_buffer();
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

unsigned long long mysql_bind::value_as_u64(int range) const {
  if (_bind[range].buffer_type == MYSQL_TYPE_LONGLONG)
    return *static_cast<uint32_t*>(_bind[range].buffer);
  else if (_bind[range].buffer_type == MYSQL_TYPE_STRING) {
    unsigned long long retval(
        strtoul(static_cast<char*>(_bind[range].buffer), nullptr, 10));
    return retval;
  } else
    assert("This field is not an unsigned long int" == nullptr);
}

/**
 *  This method is called from the statement and not directly. It is called
 *  by mysql_stmt.bind_value_as_f32() to bind the value at index range with
 *  the given value.
 *
 * @param range The index
 * @param value The float value.
 */
void mysql_bind::set_value_as_f32(int range, float value) {
  if (std::isinf(value) || std::isnan(value)) {
    set_value_as_null(range);
    return;
  }
  assert(static_cast<uint32_t>(range) < _bind.size());
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_FLOAT);
  _column[range].set_value<float>(value);
  _bind[range].buffer = _column[range].get_buffer();
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

float mysql_bind::value_as_f32(int range) const {
  if (_bind[range].buffer_type == MYSQL_TYPE_FLOAT)
    return *static_cast<float*>(_bind[range].buffer);
  else if (_bind[range].buffer_type == MYSQL_TYPE_STRING) {
    double retval(strtof(static_cast<char*>(_bind[range].buffer), nullptr));
    return retval;
  } else
    assert("This field is not a float" == nullptr);
}

/**
 *  This method is called from the statement and not directly. It is called
 *  by mysql_stmt.bind_value_as_f64() to bind the value at index range with
 *  the given value.
 *
 * @param range The index
 * @param value The double value.
 */
void mysql_bind::set_value_as_f64(int range, double value) {
  if (std::isinf(value) || std::isnan(value)) {
    set_value_as_null(range);
    return;
  }
  assert(static_cast<uint32_t>(range) < _bind.size());
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_DOUBLE);
  _column[range].set_value<double>(value);
  _bind[range].buffer = _column[range].get_buffer();
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

double mysql_bind::value_as_f64(int range) const {
  if (_bind[range].buffer_type == MYSQL_TYPE_DOUBLE)
    return *static_cast<double*>(_bind[range].buffer);
  else if (_bind[range].buffer_type == MYSQL_TYPE_STRING) {
    double retval(strtod(static_cast<char*>(_bind[range].buffer), nullptr));
    return retval;
  } else
    assert("This field is not a doube" == nullptr);
}

void mysql_bind::set_value_as_null(int range) {
  assert(static_cast<uint32_t>(range) < _bind.size());
  // if (range >= _bind.size())
  //  set_size(range + 1);
  if (!_prepared(range))
    _prepare_type(range, MYSQL_TYPE_NULL);
  //_bind[range].buffer_type = MYSQL_TYPE_NULL;
  // FIXME DBR; If data are given in column-wise, this type should fail and
  // we will have to set the is_null vector.
  _bind[range].is_null = _column[range].is_null_buffer();
  _bind[range].length = _column[range].length_buffer();
}

bool mysql_bind::value_is_null(int range) const {
  return (_column[range].is_null() ||
          _bind[range].buffer_type == MYSQL_TYPE_NULL);
}

void mysql_bind::debug() {
  std::cout << "DEBUG BIND " << this << std::endl;
  int size(_bind.size());
  for (int i(0); i < size; ++i) {
    switch (_bind[i].buffer_type) {
      case MYSQL_TYPE_LONGLONG: {
        std::cout << "LONGLONG : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length
                  << " NULL: " << (*_bind[i].is_null ? "1" : "0") << " : "
                  << *static_cast<long long*>(_column[i].get_buffer())
                  << std::endl;
      } break;
      case MYSQL_TYPE_LONG: {
        std::cout << "LONG : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length
                  << " NULL: " << (*_bind[i].is_null ? "1" : "0") << " : "
                  << *static_cast<int*>(_column[i].get_buffer()) << std::endl;
      } break;
      case MYSQL_TYPE_TINY: {
        std::cout << "TINY : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length
                  << " NULL: " << (*_bind[i].is_null ? "1" : "0") << " : "
                  << *static_cast<char*>(_column[i].get_buffer()) << std::endl;
      } break;
      case MYSQL_TYPE_NULL:
        std::cout << "NULL : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length;
        break;
      case MYSQL_TYPE_ENUM:
        std::cout << "ENUM : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length << " : "
                  << *static_cast<char**>(_column[i].get_buffer()) << std::endl;
        break;
      case MYSQL_TYPE_STRING:
        std::cout << "STRING : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length
                  << " NULL: " << (*_bind[i].is_null ? "1" : "0") << " : "
                  << *static_cast<char**>(_column[i].get_buffer()) << std::endl;
        break;
      case MYSQL_TYPE_DOUBLE: {
        std::cout << "DOUBLE : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length
                  << " NULL: " << (*_bind[i].is_null ? "1" : "0") << " : "
                  << *static_cast<double*>(_column[i].get_buffer())
                  << std::endl;
      } break;
      case MYSQL_TYPE_FLOAT: {
        std::cout << "FLOAT : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length
                  << " NULL: " << (*_bind[i].is_null ? "1" : "0") << " : "
                  << *static_cast<float*>(_column[i].get_buffer()) << std::endl;
      } break;
      default:
        std::cout << _bind[i].buffer_type << " : "
                  << " : "
                  << "BL: " << _bind[i].buffer_length << " : "
                  << "TYPE NOT MANAGED...\n";
        assert(1 == 0);  // Should not arrive...
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
