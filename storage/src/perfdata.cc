/*
** Copyright 2011 Merethis
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

#include <math.h>
#include "com/centreon/broker/storage/perfdata.hh"

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
    _max(NAN),
    _min(NAN),
    _value(0.0),
    _warning(NAN) {}

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
perfdata::~perfdata() {}

/**
 *  Assignment operator.
 *
 *  @param[in] pd Object to copy.
 *
 *  @return This object.
 */
perfdata& perfdata::operator=(perfdata const& pd) {
  _critical = pd._critical;
  _max = pd._max;
  _min = pd._min;
  _name = pd._name;
  _unit = pd._unit;
  _value = pd._value;
  _warning = pd._warning;
  return (*this);
}

/**
 *  Get the critical value.
 *
 *  @return Critical value.
 */
double perfdata::critical() const throw () {
  return (_critical);
}

/**
 *  Set the critical value.
 *
 *  @param[in] c New critical value.
 */
void perfdata::critical(double c) {
  _critical = c;
  return ;
}

/**
 *  Get the maximum value.
 *
 *  @return Maximum value.
 */
double perfdata::max() const throw () {
  return (_max);
}

/**
 *  Set the maximum value.
 *
 *  @param[in] m New maximum value.
 */
void perfdata::max(double m) {
  _max = m;
  return ;
}

/**
 *  Get the minimum value.
 *
 *  @return Minimum value.
 */
double perfdata::min() const throw () {
  return (_min);
}

/**
 *  Set the minimum value.
 *
 *  @param[in] m New minimum value.
 */
void perfdata::min(double m) {
  _min = m;
  return ;
}

/**
 *  Get the name of the metric.
 *
 *  @return Name of the metric.
 */
QString const& perfdata::name() const throw () {
  return (_name);
}

/**
 *  Set the name of the metric.
 *
 *  @param[in] n New name of the metric.
 */
void perfdata::name(QString const& n) {
  _name = n;
  return ;
}

/**
 *  Get the unit.
 *
 *  @return Unit.
 */
QString const& perfdata::unit() const throw () {
  return (_unit);
}

/**
 *  Set the unit.
 *
 *  @param[in] u New unit.
 */
void perfdata::unit(QString const& u) {
  _unit = u;
  return ;
}

/**
 *  Get the value.
 *
 *  @return Metric value.
 */
double perfdata::value() const throw () {
  return (_value);
}

/**
 *  Set the value.
 *
 *  @param[in] v New value.
 */
void perfdata::value(double v) {
  _value = v;
  return ;
}

/**
 *  Get the warning value.
 *
 *  @return Warning value.
 */
double perfdata::warning() const throw () {
  return (_warning);
}

/**
 *  Set the warning value.
 *
 *  @param[in] v New warning value.
 */
void perfdata::warning(double w) {
  _warning = w;
  return ;
}
