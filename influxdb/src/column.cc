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

#include "com/centreon/broker/influxdb/column.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

column::column()
  : _is_flag(false) {}

column::column(
          std::string const& name,
          std::string const& value,
          bool is_flag,
          type col_type)
  : _name(name),
    _value(value),
    _is_flag(is_flag),
    _type(col_type) {}

column::column(column const& c) {
  _name = c._name;
  _value = c._value;
  _is_flag = c._is_flag;
  _type = c._type;
}

column& column::operator=(column const& c) {
  if (this != &c) {
    _name = c._name;
    _value = c._value;
    _is_flag = c._is_flag;
    _type = c._type;
  }
  return (*this);
}

std::string const& column::get_name() const {
  return (_name);
}

std::string const& column::get_value() const {
  return (_value);
}

bool column::is_flag() const {
  return (_is_flag);
}

column::type column::get_type() const {
  return (_type);
}
