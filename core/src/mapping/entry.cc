/*
** Copyright 2011 Centreon
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

using namespace com::centreon::broker::mapping;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
entry::entry() : _ptr(NULL), _type(source::UNKNOWN) {}

/**
 *  Copy constructor.
 *
 *  @param[in] e Object to copy.
 */
entry::entry(entry const& e)
  : _name(e._name), _ptr(e._ptr), _source(e._source), _type(e._type) {}

/**
 *  Destructor.
 */
entry::~entry() {}

/**
 *  Assignment operator.
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
entry& entry::operator=(entry const& e) {
  _name = e._name;
  _ptr = e._ptr;
  _source = e._source;
  _type = e._type;
  return (*this);
}

/**
 *  Get the boolean value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The boolean value.
 */
bool entry::get_bool(io::data const& d) {
  return (_ptr->get_bool(d));
}

/**
 *  Get the double value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The double value.
 */
double entry::get_double(io::data const& d) {
  return (_ptr->get_double(d));
}

/**
 *  Get the integer value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The integer value.
 */
int entry::get_int(io::data const& d) {
  return (_ptr->get_int(d));
}

/**
 *  Get the short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The short value.
 */
short entry::get_short(io::data const& d) {
  return (_ptr->get_short(d));
}

/**
 *  Get the string value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The string value.
 */
QString const& entry::get_string(io::data const& d) {
  return (_ptr->get_string(d));
}

/**
 *  Get the time value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The time value.
 */
time_t entry::get_time(io::data const& d) {
  return (_ptr->get_time(d));
}

/**
 *  Get the unsigned integer value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned integer value.
 */
unsigned int entry::get_uint(io::data const& d) {
  return (_ptr->get_uint(d));
}

/**
 *  Get the unsigned short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned short value.
 */
unsigned short entry::get_ushort(io::data const& d) {
  return (_ptr->get_ushort(d));
}

/**
 *  Set the boolean value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_bool(io::data& d, bool value) {
  _ptr->set_bool(d, value);
  return ;
}

/**
 *  Set the double value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_double(io::data& d, double value) {
  _ptr->set_double(d, value);
  return ;
}

/**
 *  Set the integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_int(io::data& d, int value) {
  _ptr->set_int(d, value);
  return ;
}

/**
 *  Set the short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_short(io::data& d, short value) {
  _ptr->set_short(d, value);
  return ;
}

/**
 *  Set the string value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_string(io::data& d, QString const& value) {
  _ptr->set_string(d, value);
  return ;
}

/**
 *  Set the time value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_time(io::data& d, time_t value) {
  _ptr->set_time(d, value);
  return ;
}

/**
 *  Set the unsigned integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_uint(io::data& d, unsigned int value) {
  _ptr->set_uint(d, value);
  return ;
}

/**
 *  Set the unsigned short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_ushort(io::data& d, unsigned short value) {
  _ptr->set_ushort(d, value);
  return ;
}
