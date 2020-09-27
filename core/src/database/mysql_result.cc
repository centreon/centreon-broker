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
#include "com/centreon/broker/database/mysql_result.hh"

#include <cstdlib>

#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

/**
 *  Constructor
 *
 * @param parent The connection asking for this result
 * @param statement_id The statement id or 0 if it comes from a query.
 */
mysql_result::mysql_result(mysql_connection* parent, int statement_id)
    : _parent(parent),
      _result(nullptr, mysql_free_result),
      _row(nullptr),
      _statement_id(statement_id) {}

/**
 *  Constructor
 *
 * @param parent The connection asking for this result
 * @param result The result returned by the Mariadb Connector.
 */
mysql_result::mysql_result(mysql_connection* parent, MYSQL_RES* result)
    : _parent(parent), _result(result, mysql_free_result), _statement_id(0) {}

/**
 *  Move Constructor
 *
 * @param other Another result to move into this one.
 */
mysql_result::mysql_result(mysql_result&& other)
    : _parent(other._parent),
      _result(other._result),
      _row(other._row),
      _statement_id(other._statement_id) {
  other._row = nullptr;
  other._result = nullptr;
  other._parent = nullptr;
  _bind = move(other._bind);
}

/**
 *  Result copy operator.
 *
 * @param other
 *
 * @return this object.
 */
mysql_result& mysql_result::operator=(mysql_result const& other) {
  _result = other._result;
  _statement_id = other._statement_id;
  return *this;
}

/**
 *  Destructor
 */
mysql_result::~mysql_result() {}

/**
 *  Affects the result comming from the mariadb connector.
 *
 * @param result The result to affect.
 */
void mysql_result::set(MYSQL_RES* result) {
  _result.reset(result, mysql_free_result);
}

/**
 *  Accessor to the mariadb connector result.
 *
 *  @return A pointer to the result.
 */
MYSQL_RES* mysql_result::get() {
  return _result.get();
}

/**
 *  Accessor to a column boolean value
 *
 * @param idx The index of the column
 *
 * @return a boolean
 */
bool mysql_result::value_as_bool(int idx) {
  bool retval;
  if (_bind)
    retval = _bind->value_as_bool(idx);
  else if (_row)
    retval = _row[idx] ? strtol(_row[idx], nullptr, 10) : 0;
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Accessor to a column string value
 *
 * @param idx The index of the column
 *
 * @return a string
 */
std::string mysql_result::value_as_str(int idx) {
  std::string retval;
  if (_bind)
    retval = _bind->value_as_str(idx);
  else if (_row)
    retval = (_row[idx]) ? _row[idx] : "";
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Accessor to a column float value
 *
 * @param idx The index of the column
 *
 * @return a float
 */
float mysql_result::value_as_f32(int idx) {
  float retval;
  if (_bind)
    retval = _bind->value_as_f32(idx);
  else if (_row)
    retval = _row[idx] ? atof(_row[idx]) : 0;
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Accessor to a column double value
 *
 * @param idx The index of the column
 *
 * @return a double
 */
double mysql_result::value_as_f64(int idx) {
  double retval;
  if (_bind)
    retval = _bind->value_as_f64(idx);
  else if (_row)
    retval = _row[idx] ? atof(_row[idx]) : 0;
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Accessor to a column int value
 *
 * @param idx The index of the column
 *
 * @return an int
 */
int mysql_result::value_as_i32(int idx) {
  int retval;
  if (_bind)
    retval = _bind->value_as_i32(idx);
  else if (_row)
    retval = _row[idx] ? strtol(_row[idx], nullptr, 10) : 0;
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Accessor to a column uint32_t value
 *
 * @param idx The index of the column
 *
 * @return an uint32_t
 */
uint32_t mysql_result::value_as_u32(int idx) {
  uint32_t retval;
  if (_bind)
    retval = _bind->value_as_u32(idx);
  else if (_row)
    retval = _row[idx] ? strtoul(_row[idx], nullptr, 10) : 0;
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Accessor to a column unsigned long int value
 *
 * @param idx The index of the column
 *
 * @return an unsigned long int
 */
unsigned long long mysql_result::value_as_u64(int idx) {
  unsigned long long retval;
  if (_bind)
    retval = _bind->value_as_u64(idx);
  else if (_row)
    retval = _row[idx] ? strtoull(_row[idx], nullptr, 10) : 0;
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Tells if the column contains a null value
 *
 * @param idx The index of the column
 *
 * @return a boolean, true if the value is null.
 */
bool mysql_result::value_is_null(int idx) {
  bool retval;
  if (_bind)
    retval = _bind->value_is_null(idx);
  else if (_row)
    retval = (_row[idx] == nullptr);
  else
    throw exceptions::msg() << "mysql: No row fetched in result";
  return retval;
}

/**
 *  Tells if the result (containing a row) is empty.
 *
 * @return true if it is empty, false otherwise.
 */
bool mysql_result::is_empty() const {
  if (_bind)
    return _bind->is_empty();
  else
    return _row == nullptr;
}

/**
 *  returns the number of rows.
 *
 * @return an integer.
 */
int mysql_result::get_rows_count() const {
  if (_bind)
    return _bind->get_rows_count();
  else
    return mysql_num_rows(_result.get());
}

/**
 *  Affects a bind to the result. In other words, the values.
 *
 * @param bind The bind to set to this result.
 */
void mysql_result::set_bind(std::unique_ptr<database::mysql_bind>&& bind) {
  _bind = std::move(bind);
}

/**
 *  Affects a row from the Mariadb connector to this object.
 *
 * @param row The row to affect
 */
void mysql_result::set_row(MYSQL_ROW row) {
  _row = row;
}

/**
 *  Accessord to the bind
 *
 * @return the bind
 */
std::unique_ptr<database::mysql_bind>& mysql_result::get_bind() {
  return _bind;
}

/**
 *  Accessor to the statement id
 *
 * @return an integer
 */
int mysql_result::get_statement_id() const {
  return _statement_id;
}

/**
 *  Accessor to the parent connection.
 *
 * @return A pointer the the connection.
 */
mysql_connection* mysql_result::get_connection() {
  return _parent;
}

int mysql_result::get_num_fields() const {
  return mysql_num_fields(_result.get());
}

char const* mysql_result::get_field_name(int idx) const {
  MYSQL_FIELD* fields = mysql_fetch_fields(_result.get());
  return fields[idx].name;
}
