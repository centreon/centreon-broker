/*
** Copyright 2011,2015 Merethis
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

#include <cstddef>
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::mapping;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
entry::entry()
  : _attribute(NULL_ON_NOTHING),
    _number(0),
    _ptr(NULL),
    _type(source::UNKNOWN) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
entry::entry(entry const& other)
  : _name(other._name),
    _number(other._number),
    _ptr(other._ptr),
    _source(other._source),
    _type(other._type) {}

/**
 *  Destructor.
 */
entry::~entry() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
entry& entry::operator=(entry const& other) {
  _name = other._name;
  _number = other._number;
  _ptr = other._ptr;
  _source = other._source;
  _type = other._type;
  return (*this);
}

/**
 *  Get entry attribute.
 *
 *  @return Entry attribute.
 */
entry::attribute entry::get_attribute() const {
  return (_attribute);
}

/**
 *  Get the boolean value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The boolean value.
 */
bool entry::get_bool(io::data const& d) const {
  return (_ptr->get_bool(d));
}

/**
 *  Get the double value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The double value.
 */
double entry::get_double(io::data const& d) const {
  return (_ptr->get_double(d));
}

/**
 *  Get the integer value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The integer value.
 */
int entry::get_int(io::data const& d) const {
  return (_ptr->get_int(d));
}

/**
 *  Get entry number.
 *
 *  @return Entry number.
 */
unsigned int entry::get_number() const {
  return (_number);
}

/**
 *  Get the short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The short value.
 */
short entry::get_short(io::data const& d) const {
  return (_ptr->get_short(d));
}

/**
 *  Get the string value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The string value.
 */
QString const& entry::get_string(io::data const& d) const {
  return (_ptr->get_string(d));
}

/**
 *  Get the time value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The time value.
 */
timestamp const& entry::get_time(io::data const& d) const {
  return (_ptr->get_time(d));
}

/**
 *  Get entry type.
 *
 *  @return Entry type.
 */
unsigned int entry::get_type() const {
  return (_type);
}

/**
 *  Get the unsigned integer value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned integer value.
 */
unsigned int entry::get_uint(io::data const& d) const {
  return (_ptr->get_uint(d));
}

/**
 *  Get the unsigned short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned short value.
 */
unsigned short entry::get_ushort(io::data const& d) const {
  return (_ptr->get_ushort(d));
}

/**
 *  Set the boolean value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_bool(io::data& d, bool value) const {
  _ptr->set_bool(d, value);
  return ;
}

/**
 *  Set the double value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_double(io::data& d, double value) const {
  _ptr->set_double(d, value);
  return ;
}

/**
 *  Set the integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_int(io::data& d, int value) const {
  _ptr->set_int(d, value);
  return ;
}

/**
 *  Set the short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_short(io::data& d, short value) const {
  _ptr->set_short(d, value);
  return ;
}

/**
 *  Set the string value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_string(io::data& d, QString const& value) const {
  _ptr->set_string(d, value);
  return ;
}

/**
 *  Set the time value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_time(io::data& d, timestamp const& value) const {
  _ptr->set_time(d, value);
  return ;
}

/**
 *  Set the unsigned integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_uint(io::data& d, unsigned int value) const {
  _ptr->set_uint(d, value);
  return ;
}

/**
 *  Set the unsigned short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_ushort(io::data& d, unsigned short value) const {
  _ptr->set_ushort(d, value);
  return ;
}

/**
 *  Get if this entry is a null entry.
 *
 *  @return  True if this entry is a null entry (last entry).
 */
bool entry::is_null() const {
  return (_type == source::UNKNOWN);
}

/**
 *  Get the name of this entry.
 *
 *  @return  The name of this entry.
 */

std::string const& entry::get_name() const {
  return (_name.toStdString());
}
