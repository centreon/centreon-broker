/*
** Copyright 2009-2013,2015 Merethis
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

#include "com/centreon/broker/neb/ceof_writer.hh"

using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
ceof_writer::ceof_writer() {

}

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
ceof_writer::~ceof_writer() throw() {

}

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
