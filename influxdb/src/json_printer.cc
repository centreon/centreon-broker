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

#include <sstream>
#include "com/centreon/broker/influxdb/json_printer.hh"

using namespace com::centreon::broker::influxdb;

/**
 *  Default constructor.
 */
json_printer::json_printer() {
  _data.reserve(128);
}

/**
 *  Destructor.
 */
json_printer::~json_printer() {

}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
json_printer::json_printer(json_printer const& other) {
  _data = other._data;
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           A reference to this object.
 */
json_printer& json_printer::operator=(json_printer const& other) {
  if (this != &other) {
    _data = other._data;
  }
  return (*this);
}

/**
 *  Get the resulting string.
 *
 *  @return  The resulting string.
 */
std::string const& json_printer::get_data() const {
  return (_data);
}

/**
 *  Get the size of the resulting string.
 *
 *  @return The size of the resulting string.
 */
size_t json_printer::get_size() const {
  return (_data.size());
}

/**
 *  Open an object.
 *
 *  @param[in] name  The name of the object.
 *
 *  @return          A reference to this object.
 */
json_printer& json_printer::open_object(std::string const& name) {
  if (!name.empty())
    _data.append("\"").append(name).append("\":\"");
  _data.append("{");
  return (*this);
}

/**
 *  Close an object.
 *
 *  @return          A reference to this object.
 */
json_printer& json_printer::close_object() {
  _data.append("},");
  return (*this);
}

/**
 *  Open an array.
 *
 *  @param[in] name  The name of the array.
 *
 *  @return          A reference to this object.
 */
json_printer& json_printer::open_array(std::string const& name) {
  _data.append("\"").append(name).append("\":\"");
  _data.append("[");
  return (*this);
}

/**
 *  Close an object.
 *
 *  @return          A reference to this object.
 */
json_printer& json_printer::close_array() {
  _data.append("]");
  return (*this);
}

/**
 *  Add a string value.
 *
 *  @param[in] name   The name of the value.
 *  @param[in] value  The value.
 *
 *  @return           A reference to this object.
 */
json_printer& json_printer::add_string(
                std::string const& name,
                std::string const& value) {
  _data.append("\"").append(name).append("\":\"").append(value).append("\",");
  return (*this);
}
