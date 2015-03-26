/*
** Copyright 2011-2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/influxdb/column.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

/**
 *  Create an empty column.
 */
column::column()
  : _is_flag(false) {}

/**
 *  Create a column.
 *
 *  @param[in] name      The name of the column.
 *  @param[in] value     The value of the column.
 *  @param[in] is_flag   Is the column a flag?
 *  @param[in] col_type  Is the value of this column a string or a number?
 */
column::column(
          std::string const& name,
          std::string const& value,
          bool is_flag,
          type col_type)
  : _name(name),
    _value(value),
    _is_flag(is_flag),
    _type(col_type) {}

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
  return (*this);
}

/**
 *  Get the name of this column.
 *
 *  @return  The name of this column.
 */
std::string const& column::get_name() const {
  return (_name);
}

/**
 *  Get the value of this column.
 *
 *  @return  The value of this column.
 */
std::string const& column::get_value() const {
  return (_value);
}

/**
 *  Is this column a flag?
 *
 *  @return  True if this column is a flag.
 */
bool column::is_flag() const {
  return (_is_flag);
}

/**
 *  Get the type of this column.
 *
 *  @return  The type of this column.
 */
column::type column::get_type() const {
  return (_type);
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
    return (string);
  else if (type == "number")
    return (number);
  throw (exceptions::msg()
         << "influxdb: couldn't parse column type '" << type << "'");
}
