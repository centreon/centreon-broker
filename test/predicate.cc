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

#include "test/predicate.hh"
#include <cmath>
#include <cstring>

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**
 *  @brief Default constructor.
 *
 *  Build an invalid predicate.
 */
predicate::predicate() : _type(type_invalid) {}

/**
 *  @brief Constructor.
 *
 *  Match predicate against a null value.
 */
predicate::predicate(predicate::value_type t) : _type(type_null) {
  (void)t;
}

/**
 *  Constructor.
 *
 *  @param[in] val  Boolean value.
 */
predicate::predicate(bool val) : _range(false), _type(type_bool) {
  _val1.bval = val;
}

/**
 *  Constructor.
 *
 *  @param[in] val  Double value.
 */
predicate::predicate(double val) : _range(false), _type(type_double) {
  _val1.dval = val;
}

/**
 *  Constructor.
 *
 *  @param[in] val  Integer value.
 */
predicate::predicate(int val) : _range(false), _type(type_int) {
  _val1.ival = val;
}

/**
 *  Constructor.
 *
 *  @param[in] val  String value.
 */
predicate::predicate(char const* val) : _range(false), _type(type_string) {
  _val1.sval = val;
}

/**
 *  Constructor.
 *
 *  @param[in] val  Time value.
 */
predicate::predicate(time_t val) : _range(false), _type(type_timet) {
  _val1.tval = val;
}

/**
 *  Constructor.
 *
 *  @param[in] val  Unsigned integer value.
 */
predicate::predicate(unsigned int val) : _range(false), _type(type_uint) {
  _val1.uival = val;
}

/**
 *  Constructor.
 *
 *  @param[in] val1  First value.
 *  @param[in] val2  Second value.
 */
predicate::predicate(time_t val1, time_t val2)
    : _range(true), _type(type_timet) {
  _val1.tval = val1;
  _val2.tval = val2;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
predicate::predicate(predicate const& other)
    : _range(other._range), _type(other._type) {
  memcpy(&_val1, &other._val1, sizeof(_val1));
  memcpy(&_val2, &other._val2, sizeof(_val2));
}

/**
 *  Destructor.
 */
predicate::~predicate() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
predicate& predicate::operator=(predicate const& other) {
  if (this != &other) {
    _range = other._range;
    _type = other._type;
    _val1 = other._val1;
    _val2 = other._val2;
  }
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if other object matches this predicate.
 */
bool predicate::operator==(predicate const& other) const {
  return ((_range == other._range) && (_type == other._type) &&
          !memcmp(&_val1, &other._val1, sizeof(_val1)) &&
          !memcmp(&_val2, &other._val2, sizeof(_val2)));
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if other object matches this predicate.
 */
bool predicate::operator==(QVariant const& other) const {
  bool retval;
  if (!is_valid())
    retval = false;
  else if (is_null())
    retval = other.isNull();
  else if (other.isNull())
    retval = false;
  else if (is_range()) {
    if (_type == type_double) {
      double d(other.toDouble());
      retval = (d >= _val1.dval) && (d <= _val2.dval);
    } else if (_type == type_int) {
      int i(other.toInt());
      retval = (i >= _val1.ival) && (i <= _val2.ival);
    } else if (_type == type_timet) {
      time_t t(other.toLongLong());
      retval = (t >= _val1.tval) && (t <= _val2.tval);
    } else if (_type == type_uint) {
      unsigned int u(other.toUInt());
      retval = (u >= _val1.uival) && (u <= _val2.uival);
    } else
      retval = false;
  } else if (_type == type_bool)
    retval = (other.toBool() == _val1.bval);
  else if (_type == type_double) {
    double d(other.toDouble());
    retval = (isnan(d) && isnan(_val1.dval)) ||
             (isinf(d) && isinf(_val1.dval) &&
              (std::signbit(d) == std::signbit(_val1.dval))) ||
             (std::isfinite(d) && std::isfinite(_val1.dval) &&
              !(fabs(d - _val1.dval) > (0.01 * fabs(_val1.dval))));
  } else if (_type == type_int)
    retval = (other.toInt() == _val1.ival);
  else if (_type == type_string)
    retval = (other.toString() == _val1.sval);
  else if (_type == type_timet)
    retval = (other.toLongLong() == _val1.tval);
  else if (_type == type_uint)
    retval = (other.toUInt() == _val1.uival);
  else
    retval = false;
  return (retval);
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if other object does not match this predicate.
 */
bool predicate::operator!=(predicate const& other) const {
  return (!operator==(other));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if other object does not match this predicate.
 */
bool predicate::operator!=(QVariant const& other) const {
  return (!operator==(other));
}

/**
 *  Get (first) value.
 *
 *  @return First value.
 */
predicate::uval const& predicate::get_value() const {
  return (_val1);
}

/**
 *  Get second value.
 *
 *  @return Second value.
 */
predicate::uval const& predicate::get_value2() const {
  return (_val2);
}

/**
 *  Get value type.
 *
 *  @return Value type.
 */
predicate::value_type predicate::get_value_type() const {
  return (_type);
}

/**
 *  Check if the predicate is null.
 *
 *  @return True if the predicate is null.
 */
bool predicate::is_null() const {
  return (_type == type_null);
}

/**
 *  Check if the predicate is a range.
 *
 *  @return True if the predicate is a range.
 */
bool predicate::is_range() const {
  return (_range);
}

/**
 *  Check if the predicate is valid.
 *
 *  @return True if the predicate is valid.
 */
bool predicate::is_valid() const {
  return (_type != type_invalid);
}

/**
 *  Print a predicate to a stringifier.
 *
 *  @param[out] s  Stringifier.
 *  @param[in]  p  Predicate.
 *
 *  @return The stringifier object.
 */
misc::stringifier& operator<<(misc::stringifier& s, predicate const& p) {
  if (!p.is_valid())
    s << "(invalid)";
  else if (p.is_null())
    s << "NULL";
  else if (p.is_range()) {
    if (p.get_value_type() == predicate::type_bool)
      s << (p.get_value().bval ? "true" : "false") << "-"
        << (p.get_value2().bval ? "true" : "false");
    else if (p.get_value_type() == predicate::type_double)
      s << p.get_value().dval << "-" << p.get_value2().dval;
    else if (p.get_value_type() == predicate::type_int)
      s << p.get_value().ival << "-" << p.get_value2().ival;
    else if (p.get_value_type() == predicate::type_timet)
      s << p.get_value().tval << "-" << p.get_value2().tval;
    else if (p.get_value_type() == predicate::type_uint)
      s << p.get_value().uival << "-" << p.get_value2().uival;
    else
      s << "(unsupported)";
  } else if (p.get_value_type() == predicate::type_bool)
    s << (p.get_value().bval ? "true" : "false");
  else if (p.get_value_type() == predicate::type_double)
    s << p.get_value().dval;
  else if (p.get_value_type() == predicate::type_int)
    s << p.get_value().ival;
  else if (p.get_value_type() == predicate::type_string)
    s << p.get_value().sval;
  else if (p.get_value_type() == predicate::type_timet)
    s << p.get_value().tval;
  else if (p.get_value_type() == predicate::type_uint)
    s << p.get_value().uival;
  else
    s << "(unsupported)";
  return (s);
}
