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

#include "com/centreon/broker/mapping/entry.hh"
#include <cstddef>

using namespace com::centreon::broker;
using namespace com::centreon::broker::mapping;

/**
 *  Get the boolean value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The boolean value.
 */
bool entry::get_bool(io::data const& d) const {
  return _source->get_bool(d);
}

/**
 *  Get the double value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The double value.
 */
double entry::get_double(io::data const& d) const {
  return _source->get_double(d);
}

/**
 *  Get the integer value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The integer value.
 */
int entry::get_int(io::data const& d) const {
  return _source->get_int(d);
}

/**
 *  Get the short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The short value.
 */
short entry::get_short(io::data const& d) const {
  return _source->get_short(d);
}

/**
 *  Get the string value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The string value.
 */
std::string const& entry::get_string(io::data const& d, size_t* max_len) const {
  return _source->get_string(d, max_len);
}

/**
 *  Get the time value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The time value.
 */
timestamp const& entry::get_time(io::data const& d) const {
  return _source->get_time(d);
}

/**
 *  Get the uint32_teger value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The uint32_teger value.
 */
uint32_t entry::get_uint(io::data const& d) const {
  return _source->get_uint(d);
}

/**
 *  Get the uint64_teger value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The uint32_teger value.
 */

uint64_t entry::get_ulong(io::data const& d) const {
  return _source->get_ulong(d);
}

/**
 *  Get the unsigned short value.
 *
 *  @param[in] d Object to work on.
 *
 *  @return The unsigned short value.
 */
unsigned short entry::get_ushort(io::data const& d) const {
  return _source->get_ushort(d);
}

/**
 *  Set the boolean value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_bool(io::data& d, bool value) const {
  _source->set_bool(d, value);
}

/**
 *  Set the double value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_double(io::data& d, double value) const {
  _source->set_double(d, value);
}

/**
 *  Set the integer value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_int(io::data& d, int value) const {
  _source->set_int(d, value);
}

/**
 *  Set the short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_short(io::data& d, short value) const {
  _source->set_short(d, value);
}

/**
 *  Set the string value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_string(io::data& d, std::string const& value) const {
  _source->set_string(d, value);
}

/**
 *  Set the time value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_time(io::data& d, timestamp const& value) const {
  _source->set_time(d, value);
}

/**
 *  Set the uint32_teger value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_uint(io::data& d, uint32_t value) const {
  _source->set_uint(d, value);
}

/**
 *  Set the uint64_teger value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_ulong(io::data& d, uint64_t value) const {
  _source->set_ulong(d, value);
}

/**
 *  Set the unsigned short value.
 *
 *  @param[out] d     Object to work on.
 *  @param[in]  value New value.
 */
void entry::set_ushort(io::data& d, unsigned short value) const {
  _source->set_ushort(d, value);
}
