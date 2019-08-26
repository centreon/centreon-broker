/*
** Copyright 2011,2015 Centreon
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
  : _attribute(always_valid),
    _name(NULL),
    _name_v2(NULL),
    _ptr(NULL),
    _serialize(false),
    _type(source::UNKNOWN) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
entry::entry(entry const& other)
  : _name(other._name),
    _name_v2(other._name_v2),
    _ptr(other._ptr),
    _serialize(other._serialize),
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
  if (this != &other) {
    _name = other._name;
    _name_v2 = other._name_v2;
    _ptr = other._ptr;
    _serialize = other._serialize;
    _source = other._source;
    _type = other._type;
  }
  return (*this);
}

/**
 *  Get entry attribute.
 *
 *  @return Entry attribute.
 */
unsigned int entry::get_attribute() const {
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
 *  Get the name of this entry.
 *
 *  @return The name of this entry.
 */
char const* entry::get_name() const {
  return (_name);
}

/**
 *  Get the name of this entry in version 2.x.
 *
 *  @return The name of this entry in version 2.x.
 */
char const* entry::get_name_v2() const {
  return (_name_v2);
}

/**
 *  Check if entry is to be serialized.
 *
 *  @return True if entry is to be serialized.
 */
bool entry::get_serialize() const {
  return (_serialize);
}

/**
 *  Get the short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The short value.
 */
short entry::get_short(io::data const& d) const {
  return _ptr->get_short(d);
}

/**
 *  Get the string value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The string value.
 */
std::string const& entry::get_string(io::data const& d) const {
  return _ptr->get_string(d);
}

/**
 *  Get the time value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The time value.
 */
timestamp const& entry::get_time(io::data const& d) const {
  return _ptr->get_time(d);
}

/**
 *  Get entry type.
 *
 *  @return Entry type.
 */
unsigned int entry::get_type() const {
  return _type;
}

/**
 *  Get the unsigned integer value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned integer value.
 */
unsigned int entry::get_uint(io::data const& d) const {
  return _ptr->get_uint(d);
}

/**
 *  Get the unsigned short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned short value.
 */
unsigned short entry::get_ushort(io::data const& d) const {
  return _ptr->get_ushort(d);
}

/**
 *  Get if this entry is a null entry.
 *
 *  @return  True if this entry is a null entry (last entry).
 */
bool entry::is_null() const {
  return _type == source::UNKNOWN;
}

/**
 *  Set the boolean value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_bool(io::data& d, bool value) const {
  _ptr->set_bool(d, value);
}

/**
 *  Set the double value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_double(io::data& d, double value) const {
  _ptr->set_double(d, value);
}

/**
 *  Set the integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_int(io::data& d, int value) const {
  _ptr->set_int(d, value);
}

/**
 *  Set the short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_short(io::data& d, short value) const {
  _ptr->set_short(d, value);
}

/**
 *  Set the string value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_string(io::data& d, std::string const& value) const {
  _ptr->set_string(d, value);
}

/**
 *  Set the time value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_time(io::data& d, timestamp const& value) const {
  _ptr->set_time(d, value);
}

/**
 *  Set the unsigned integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_uint(io::data& d, unsigned int value) const {
  _ptr->set_uint(d, value);
}

/**
 *  Set the unsigned short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_ushort(io::data& d, unsigned short value) const {
  _ptr->set_ushort(d, value);
}
