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
#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/mysql_result.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

mysql_result::mysql_result(int statement_id)
  : _result(NULL, mysql_free_result),
    _statement_id(statement_id),
    _row(NULL) {}

mysql_result::mysql_result(MYSQL_RES* result)
  : _result(result, mysql_free_result),
    _statement_id(0) {}

mysql_result::mysql_result(mysql_result const& other)
  : _result(other._result),
    _statement_id(other._statement_id) {}

mysql_result& mysql_result::operator=(mysql_result const& other) {
  _result = other._result;
  _statement_id = other._statement_id;
  return *this;
}

mysql_result::~mysql_result() {
  std::cout << "END OF MYSQL_RESULT";
}

void mysql_result::set(MYSQL_RES* result) {
  _result = misc::shared_ptr<MYSQL_RES>(result, mysql_free_result);
}

MYSQL_RES* mysql_result::get() {
  return _result.data();
}

bool mysql_result::value_as_bool(int idx) {
  bool retval;
  if (_bind.get())
    retval = _bind->value_as_bool(idx);
  else if (_row)
    retval = _row[idx] ? strtol(_row[idx], 0, 10) : 0;
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

std::string mysql_result::value_as_str(int idx) {
  std::string retval;
  if (_bind.get())
    retval = _bind->value_as_str(idx);
  else if (_row)
    retval = _row[idx];
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

float mysql_result::value_as_f32(int idx) {
  float retval;
  if (_bind.get())
    retval = _bind->value_as_f32(idx);
  else if (_row)
    retval = _row[idx] ? atof(_row[idx]) : 0;
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

double mysql_result::value_as_f64(int idx) {
  double retval;
  if (_bind.get())
    retval = _bind->value_as_f64(idx);
  else if (_row)
    retval = _row[idx] ? atof(_row[idx]) : 0;
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

int mysql_result::value_as_i32(int idx) {
  int retval;
  if (_bind.get())
    retval = _bind->value_as_i32(idx);
  else if (_row)
    retval = _row[idx] ? strtol(_row[idx], 0, 10) : 0;
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

unsigned int mysql_result::value_as_u32(int idx) {
  unsigned int retval;
  if (_bind.get())
    retval = _bind->value_as_u32(idx);
  else if (_row)
    retval = _row[idx] ? strtoul(_row[idx], 0, 10) : 0;
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

unsigned long long mysql_result::value_as_u64(int idx) {
  unsigned long long retval;
  if (_bind.get())
    retval = _bind->value_as_u64(idx);
  else if (_row)
    retval = _row[idx] ? strtoull(_row[idx], 0, 10) : 0;
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

bool mysql_result::value_is_null(int idx) {
  bool retval;
  if (_bind.get())
    retval = _bind->value_is_null(idx);
  else if (_row)
    retval = (_row[idx] == 0);
  else
    throw exceptions::msg()
      << "mysql: No row fetched in result";
  return retval;
}

bool mysql_result::is_empty() const {
  if (_bind.get())
    return _bind->is_empty();
  else
    return _row == NULL;
}

int mysql_result::get_rows_count() const {
  if (_bind.get())
    return _bind->get_rows_count();
  else
    return mysql_num_rows(_result.data());
}

void mysql_result::set_bind(std::auto_ptr<mysql_bind> bind) {
  _bind = bind;
}

void mysql_result::set_row(MYSQL_ROW row) {
  _row = row;
}

mysql_bind* mysql_result::get_bind() {
  return _bind.get();
}

int mysql_result::get_statement_id() const {
  return _statement_id;
}
