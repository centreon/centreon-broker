/*
 * Copyright 2019 Centreon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 */

#include <cassert>
#include "com/centreon/broker/misc/variant.hh"

using namespace com::centreon::broker::misc;

/**
 *  Default constructor
 */
variant::variant() : _type{type_none} {}

/**
 *  Constructor
 */
variant::variant(char const* value)
    : _type{type_string}, _str_value{value} {}

/**
 *  Constructor
 */
variant::variant(std::string const& value)
    : _type{type_string}, _str_value{value} {}

/**
 *  Constructor
 */
variant::variant(bool value) : _type{type_bool}, _bool_value{value} {}

/**
 *  Constructor
 */
variant::variant(int32_t value) : _type{type_int}, _int_value{value} {}

/**
 *  Constructor
 */
variant::variant(uint32_t value) : _type{type_uint}, _uint_value{value} {}

/**
 *  Constructor
 */
variant::variant(int64_t value) : _type{type_long}, _long_value{value} {}

/**
 *  Constructor
 */
variant::variant(uint64_t value) : _type{type_ulong}, _ulong_value{value} {}

/**
 *  Constructor
 */
variant::variant(double value) : _type{type_double}, _dbl_value{value} {}

/**
 *  Copy constructor
 */
variant::variant(variant const& var) : _type{var._type} {
  switch (_type) {
    case type_bool:
      _bool_value = var._bool_value;
      break;
    case type_int:
      _int_value = var._int_value;
      break;
    case type_uint:
      _uint_value = var._uint_value;
      break;
    case type_long:
      _int_value = var._int_value;
      break;
    case type_ulong:
      _uint_value = var._uint_value;
      break;
    case type_double:
      _dbl_value = var._dbl_value;
      break;
    case type_string:
      new(&_str_value) std::string(var._str_value);
      break;
    default:
      // If we enter here, it is a bug.
      assert(1 == 0);
  }
}

/**
 *  Destructor
 */
variant::~variant() {
  using std::string;
  if (_type == type_string)
    _str_value.~string();
}

/**
 *  Copy operator
 *
 * @param other The variant to copy
 */
variant& variant::operator=(variant const& other) {
  if (_type == type_string) {
    using std::string;
    _str_value.~string();
  }

  _type = other._type;
  switch (_type) {
    case type_bool:
      _bool_value = other._bool_value;
      break;
    case type_int:
      _int_value = other._int_value;
      break;
    case type_uint:
      _uint_value = other._uint_value;
      break;
    case type_long:
      _int_value = other._int_value;
      break;
    case type_ulong:
      _uint_value = other._uint_value;
      break;
    case type_double:
      _dbl_value = other._dbl_value;
      break;
    case type_string:
      new(&_str_value) std::string(other._str_value);
      break;
    default:
      // If we enter here, it is a bug.
      assert(1 == 0);
  }
  return *this;
}

/**
 *  Getter of the variant type
 *
 * @return the variant type
 */
variant::meta_type variant::user_type() const {
  return _type;
}

/**
 *  Returns the variant as a boolean. This method only works if the variant
 *  content is a boolean or none.
 *
 * @return the boolean value or false.
 */
bool variant::as_bool() const {
  assert(_type == type_int || _type == type_uint || _type == type_long ||
         _type == type_ulong || _type == type_bool || _type == type_none);
  if (_type == type_none)
    return false;
  else if (_type == type_bool)
    return _bool_value;
  else
    return static_cast<bool>(_long_value);
}

/**
 *  Returns the variant as an int32. This method only works if the variant
 *  content is an int32, an uint32 or none.
 *
 * @return the value or 0 in the case of none.
 */
int32_t variant::as_int() const {
  assert(_type == type_int || _type == type_uint || _type == type_none);
  if (_type == type_none)
    return 0;
  else
    return _int_value;
}

/**
 *  Returns the variant as an unsigned int32. This method only works if the variant
 *  content is an int32, an uint32 or none.
 *
 * @return the value or 0 in the case of none.
 */
uint32_t variant::as_uint() const {
  assert(_type == type_uint || _type == type_int || _type == type_none);
  if (_type == type_none)
    return 0;
  else
    return _uint_value;
}

/**
 *  Returns the variant as an int64. This method only works if the variant
 *  content is an int64, an uint64 or none.
 *
 * @return the value or 0 in the case of none.
 */
int64_t variant::as_long() const {
  assert(_type == type_ulong || _type == type_long || _type == type_none);
  if (_type == type_none)
    return 0;
  else
    return _long_value;
}

/**
 *  Returns the variant as an unsigned int64. This method only works if the variant
 *  content is an int64, an uint64 or none.
 *
 * @return the value or 0 in the case of none.
 */
uint64_t variant::as_ulong() const {
  assert(_type == type_ulong || _type == type_long || _type == type_none);
  if (_type == type_none)
    return 0;
  else
    return _ulong_value;
}

/**
 *  Returns the variant as a double. This method only works if the variant
 *  content is a double or none.
 *
 * @return the value or 0 in the case of none.
 */
double variant::as_double() const {
  assert(_type == type_double || _type == type_none);
  if (_type == type_none)
    return 0;
  else
    return _dbl_value;
}

/**
 *  Returns the variant as a string. This method only works if the variant
 *  content is a string.
 *
 * @return the value.
 */
std::string const& variant::as_string() const {
  assert(_type == type_string);
  return _str_value;
}
