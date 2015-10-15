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
predicate::predicate(unsigned int val)
  : _range(false), _type(type_uint) {
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
  : _range(other._range),
    _type(other._type),
    _val1(other._val1),
    _val2(other._val2) {}

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
 *  Check if the predicate is null.
 *
 *  @return True if the predicate is null.
 */
bool predicate::is_null() const {
  return (_type == type_null);
}

/**
 *  Check if the predicate is valid.
 *
 *  @return True if the predicate is valid.
 */
bool predicate::is_valid() const {
  return (_type != type_invalid);
}
