/*
** Copyright 2015 Centreon
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

#include "com/centreon/broker/misc/json_writer.hh"
#include <sstream>

using namespace com::centreon::broker::misc;

/**
 *  Default constructor.
 */
json_writer::json_writer() {}

/**
 *  Destructor.
 */
json_writer::~json_writer() {}

/**
 *  Get the constructed string.
 *
 *  @return  The constructed string.
 */
std::string const& json_writer::get_string() const {
  return (_string);
}

/**
 *  Open a json object.
 */
void json_writer::open_object() {
  _put_comma();
  _string += '{';
}

/**
 *  Close a json object.
 */
void json_writer::close_object() {
  _string += '}';
}

/**
 *  Open a json array.
 */
void json_writer::open_array() {
  _put_comma();
  _string += '[';
}

/**
 *  Close a json array.
 */
void json_writer::close_array() {
  _string += ']';
}

/**
 *  Add a key.
 *
 *  @param[in] key  The key name.
 */
void json_writer::add_key(std::string const& key) {
  add_string(key);
  _string += ":";
}

/**
 *  Add a string.
 *
 *  @param[in] str  The string.
 */
void json_writer::add_string(std::string const& str) {
  _put_comma();
  ((_string += '"') += str) += '"';
}

/**
 *  Add a number.
 *
 *  @param[in] number  The number.
 */
void json_writer::add_number(long long number) {
  _put_comma();
  std::stringstream ss;
  ss << number;
  std::string res;
  ss >> res;
  _string += res;
}

/**
 *  Add a double.
 *
 *  @param[in] number  The number in double precision.
 */
void json_writer::add_double(double number) {
  _put_comma();
  std::stringstream ss;
  ss << number;
  std::string res;
  ss >> res;
  _string += res;
}

/**
 *  Add a null.
 */
void json_writer::add_null() {
  _put_comma();
  _string += "null";
}

/**
 *  Add a boolean.
 *
 *  @param[in] value  The value of the boolean.
 */
void json_writer::add_boolean(bool value) {
  _put_comma();
  if (value)
    _string += "true";
  else
    _string += "false";
}

/**
 *  Merge a string with the actual command.
 *
 *  @param[in] val  The string to merge.
 */
void json_writer::merge(std::string const& val) {
  _string += val;
}

/**
 *  Put a comma, if needed.
 */
void json_writer::_put_comma() {
  if (!_string.empty() && _string[_string.size() - 1] != ':' &&
      _string[_string.size() - 1] != '{' && _string[_string.size() - 1] != '[')
    _string += ',';
}
