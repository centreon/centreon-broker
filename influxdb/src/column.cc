/*
** Copyright 2011-2014 Centreon
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

#include "com/centreon/broker/influxdb/column.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

/**
 *  Create an empty column.
 */
column::column() : _is_flag(false), _type{column::number} {}

/**
 *  Create a column.
 *
 *  @param[in] name      The name of the column.
 *  @param[in] value     The value of the column.
 *  @param[in] is_flag   Is the column a flag?
 *  @param[in] col_type  Is the value of this column a string or a number?
 */
column::column(std::string const& name,
               std::string const& value,
               bool is_flag,
               type col_type)
    : _name(name), _value(value), _is_flag(is_flag), _type(col_type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c  The column to copy.
 */
column::column(column const& c) {
  _name = c._name;
  _value = c._value;
  _is_flag = c._is_flag;
  _type = c._type;
}

/**
 *  Assignment operator.
 *
 *  @param[in] c  The column to copy.
 *
 *  @return       A reference to this object.
 */
column& column::operator=(column const& c) {
  if (this != &c) {
    _name = c._name;
    _value = c._value;
    _is_flag = c._is_flag;
    _type = c._type;
  }
  return *this;
}

/**
 *  Get the name of this column.
 *
 *  @return  The name of this column.
 */
std::string const& column::get_name() const {
  return _name;
}

/**
 *  Get the value of this column.
 *
 *  @return  The value of this column.
 */
std::string const& column::get_value() const {
  return _value;
}

/**
 *  Is this column a flag?
 *
 *  @return  True if this column is a flag.
 */
bool column::is_flag() const {
  return _is_flag;
}

/**
 *  Get the type of this column.
 *
 *  @return  The type of this column.
 */
column::type column::get_type() const {
  return _type;
}

/**
 *  Parse a string containing a type.
 *
 *  @param[in] type  The string containing the type.
 *
 *  @return          The type.
 */
column::type column::parse_type(std::string const& type) {
  if (type == "string")
    return string;
  else if (type == "number")
    return number;
  throw msg_fmt("influxdb: couldn't parse column type '{}'", type);
}
