/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/ceof/ceof_writer.hh"

using namespace com::centreon::broker::ceof;

/**
 *  Default constructor.
 */
ceof_writer::ceof_writer() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
ceof_writer::ceof_writer(ceof_writer const& other) {
  _str = other._str;
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return           Reference to this object.
 */
ceof_writer& ceof_writer::operator=(ceof_writer const& other) {
  if (this != &other) {
    _str = other._str;
  }
  return (*this);
}

/**
 *  Destructor.
 */
ceof_writer::~ceof_writer() throw() {}

/**
 *  Open an object.
 *
 *  @param[in] object_type  The type of the object.
 */
void ceof_writer::open_object(std::string const& object_type) {
  ((_str += "define ") += object_type) += " {\n";
}

/**
 *  Add a key of an attribute.
 *
 *  @param[in] key  The key to add.
 */
void ceof_writer::add_key(std::string const& key) {
  (_str += key) += " ";
}

/**
 *  Add the value of an attribute.
 *
 *  @param[in] value  The value to add.
 */
void ceof_writer::add_value(std::string const& value) {
  (_str += value) += "\n";
}

/**
 *  Close an object.
 */
void ceof_writer::close_object() {
  _str += "}\n";
}

/**
 *  Get the string.
 *
 *  @return  The string.
 */
std::string const& ceof_writer::get_string() const throw() {
  return (_str);
}
