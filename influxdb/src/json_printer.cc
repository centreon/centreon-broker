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

#include "com/centreon/broker/influxdb/json_printer.hh"
#include <sstream>

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
json_printer::~json_printer() {}

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
 *  Clear the printer string.
 */
void json_printer::clear() {
  _data.clear();
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
  add_tag(name);
  _data.append("{");
  return (*this);
}

/**
 *  Close an object.
 *
 *  @return          A reference to this object.
 */
json_printer& json_printer::close_object() {
  if (!_data.empty() && _data[_data.size() - 1] == ',')
    _data[_data.size() - 1] = ' ';
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
  add_tag(name);
  _data.append("[");
  return (*this);
}

/**
 *  Close an object.
 *
 *  @return          A reference to this object.
 */
json_printer& json_printer::close_array() {
  if (!_data.empty() && _data[_data.size() - 1] == ',')
    _data[_data.size() - 1] = ']';
  else
    _data.append("]");
  return (*this);
}
