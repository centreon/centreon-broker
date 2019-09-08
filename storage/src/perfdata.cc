/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/storage/perfdata.hh"
#include <cmath>

using namespace com::centreon::broker::storage;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
perfdata::perfdata()
    : _critical(NAN),
      _critical_low(NAN),
      _critical_mode(false),
      _max(NAN),
      _min(NAN),
      _value(NAN),
      _value_type(gauge),
      _warning(NAN),
      _warning_low(NAN),
      _warning_mode(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] pd Object to copy.
 */
perfdata::perfdata(perfdata const& pd) {
  operator=(pd);
}

/**
 *  Destructor.
 */
perfdata::~perfdata() throw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] pd Object to copy.
 *
 *  @return This object.
 */
perfdata& perfdata::operator=(perfdata const& pd) {
  if (this != &pd) {
    _critical = pd._critical;
    _critical_low = pd._critical_low;
    _critical_mode = pd._critical_mode;
    _max = pd._max;
    _min = pd._min;
    _name = pd._name;
    _unit = pd._unit;
    _value = pd._value;
    _value_type = pd._value_type;
    _warning = pd._warning;
    _warning_low = pd._warning_low;
    _warning_mode = pd._warning_mode;
  }
  return (*this);
}

/**
 *  Get the critical value.
 *
 *  @return Critical value.
 */
double perfdata::critical() const throw() {
  return (_critical);
}

/**
 *  Set the critical value.
 *
 *  @param[in] c New critical value.
 */
void perfdata::critical(double c) throw() {
  _critical = c;
  return;
}

/**
 *  Get the low critical threshold.
 *
 *  @return Low critical value.
 */
double perfdata::critical_low() const throw() {
  return (_critical_low);
}

/**
 *  Set the low critical threshold.
 *
 *  @param[in] c Low critical value.
 */
void perfdata::critical_low(double c) throw() {
  _critical_low = c;
  return;
}

/**
 *  Get the critical threshold mode.
 *
 *  @return false if an alert is generated if the value is outside the
 *          range, true otherwise.
 */
bool perfdata::critical_mode() const throw() {
  return (_critical_mode);
}

/**
 *  Set the critical threshold mode.
 *
 *  @param[in] m false if an alert is generated if the value is outside
 *               the range, true otherwise.
 */
void perfdata::critical_mode(bool m) throw() {
  _critical_mode = m;
  return;
}

/**
 *  Get the maximum value.
 *
 *  @return Maximum value.
 */
double perfdata::max() const throw() {
  return (_max);
}

/**
 *  Set the maximum value.
 *
 *  @param[in] m New maximum value.
 */
void perfdata::max(double m) throw() {
  _max = m;
  return;
}

/**
 *  Get the minimum value.
 *
 *  @return Minimum value.
 */
double perfdata::min() const throw() {
  return (_min);
}

/**
 *  Set the minimum value.
 *
 *  @param[in] m New minimum value.
 */
void perfdata::min(double m) throw() {
  _min = m;
  return;
}

/**
 *  Get the name of the metric.
 *
 *  @return Name of the metric.
 */
std::string const& perfdata::name() const throw() {
  return (_name);
}

/**
 *  Set the name of the metric.
 *
 *  @param[in] n New name of the metric.
 */
void perfdata::name(std::string const& n) {
  _name = n;
  return;
}

/**
 *  Get the unit.
 *
 *  @return Unit.
 */
std::string const& perfdata::unit() const throw() {
  return (_unit);
}

/**
 *  Set the unit.
 *
 *  @param[in] u New unit.
 */
void perfdata::unit(std::string const& u) {
  _unit = u;
  return;
}

/**
 *  Set the value.
 *
 *  @param[in] v New value.
 */
void perfdata::value(double v) throw() {
  _value = v;
  return;
}

/**
 *  Get the type of the value.
 *
 *  @return Type of the value.
 */
perfdata::data_type perfdata::value_type() const throw() {
  return (_value_type);
}

/**
 *  Set the type of the value.
 *
 *  @param[in] t New type.
 */
void perfdata::value_type(perfdata::data_type t) throw() {
  _value_type = t;
  return;
}

/**
 *  Get the warning value.
 *
 *  @return Warning value.
 */
double perfdata::warning() const throw() {
  return (_warning);
}

/**
 *  Set the warning value.
 *
 *  @param[in] v New warning value.
 */
void perfdata::warning(double w) throw() {
  _warning = w;
  return;
}

/**
 *  Get the low warning threshold.
 *
 *  @return Low warning value.
 */
double perfdata::warning_low() const throw() {
  return (_warning_low);
}

/**
 *  Set the low warning threshold.
 *
 *  @param[in] w Low warning value.
 */
void perfdata::warning_low(double w) throw() {
  _warning_low = w;
  return;
}

/**
 *  Get the warning threshold mode.
 *
 *  @return false if an alert is generated if the value is outside the
 *          range, true otherwise.
 */
bool perfdata::warning_mode() const throw() {
  return (_warning_mode);
}

/**
 *  Set the warning threshold mode.
 *
 *  @param[in] m false if an alert is generated if the value it outside
 *               the range, true otherwise.
 */
void perfdata::warning_mode(bool m) throw() {
  _warning_mode = m;
  return;
}

/**************************************
 *                                     *
 *          Global Functions           *
 *                                     *
 **************************************/

/**
 *  Comparison helper.
 *
 *  @param[in] a First value.
 *  @param[in] b Second value.
 *
 *  @return true if a and b are equal.
 */
static inline bool double_equal(double a, double b) {
  return ((std::isnan(a) && std::isnan(b)) ||
          (std::isinf(a) && std::isinf(b) &&
           (std::signbit(a) == std::signbit(b))) ||
          (std::isfinite(a) && std::isfinite(b) &&
           !(fabs((a) - (b)) > (0.01 * fabs(a)))));
}

/**
 *  Compare two perfdata objects.
 *
 *  @param[in] left  First object.
 *  @param[in] right Second object.
 *
 *  @return true if both objects are equal.
 */
bool operator==(perfdata const& left, perfdata const& right) {
  return (double_equal(left.critical(), right.critical()) &&
          double_equal(left.critical_low(), right.critical_low()) &&
          (left.critical_mode() == right.critical_mode()) &&
          double_equal(left.max(), right.max()) &&
          double_equal(left.min(), right.min()) &&
          (left.name() == right.name()) && (left.unit() == right.unit()) &&
          double_equal(left.value(), right.value()) &&
          (left.value_type() == right.value_type()) &&
          double_equal(left.warning(), right.warning()) &&
          double_equal(left.warning_low(), right.warning_low()) &&
          (left.warning_mode() == right.warning_mode()));
}

/**
 *  Compare two perfdata objects.
 *
 *  @param[in] left  First object.
 *  @param[in] right Second object.
 *
 *  @return true if both objects are inequal.
 */
bool operator!=(perfdata const& left, perfdata const& right) {
  return (!(left == right));
}
