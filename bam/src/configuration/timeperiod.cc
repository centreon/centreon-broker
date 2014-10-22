/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/configuration/timeperiod.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
timeperiod::timeperiod() :
  _id(0) {}

/**
 *  Construct the timeperiod from data.
 *
 *  @param[in] id          The id of the timeperiod.
 *  @param[in] name        The name of the timeperiod.
 *  @param[in] alias       The alias of the timeperiod.
 *  @param[in] sunday      A string describing the sunday timerange.
 *  @param[in] monday      A string describing the monday timerange.
 *  @param[in] tuesday     A string describing the tuesday timerange.
 *  @param[in] wednesday   A string describing the wednesday timerange.
 *  @param[in] thursday    A string describing the thursday timerange.
 *  @param[in] friday      A string describing the friday timerange.
 *  @param[in] saturday    A string describing the saturday timerange.
 */
timeperiod::timeperiod(unsigned int id,
                       std::string const& name,
                       std::string const& alias,
                       std::string const& sunday,
                       std::string const& monday,
                       std::string const& tuesday,
                       std::string const& wednesday,
                       std::string const& thursday,
                       std::string const& friday,
                       std::string const& saturday)
  : _id(id),
    _name(name),
    _alias(alias),
    _sunday(sunday),
    _monday(monday),
    _tuesday(tuesday),
    _wednesday(wednesday),
    _thursday(thursday),
    _friday(friday),
    _saturday(saturday) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to be copied.
 */
timeperiod::timeperiod(timeperiod const& other) {
  timeperiod::operator==(other);
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to be copied.
 *
 *  @return           A reference to this object.
 */
timeperiod& timeperiod::operator=(timeperiod const& other) {
  if (this != &other) {
    _id = other._id;
    _name = other._name;
    _alias = other._alias;
    _sunday = other._sunday;
    _monday = other._monday;
    _tuesday = other._tuesday;
    _wednesday = other._wednesday;
    _thursday = other._thursday;
    _friday = other._friday;
    _saturday = other._saturday;
    _exceptions = other._exceptions;
    _include = other._include;
    _exclude = other._exclude;
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to be tested for equality.
 *
 *  @return           True of both objects are equal.
 */
bool timeperiod::operator==(timeperiod const& other) const {
  return ((_id == other._id)
          && (_name == other._name)
          && (_alias == other._alias)
          && (_sunday == other._sunday)
          && (_monday == other._monday)
          && (_tuesday == other._tuesday)
          && (_wednesday == other._wednesday)
          && (_thursday == other._thursday)
          && (_friday == other._friday)
          && (_saturday == other._saturday)
          && (_exceptions == other._exceptions)
          && (_include == other._include)
          && (_exclude == other._exclude));
}

/**
 *  Add an exception to the timeperiod.
 *
 *  @param[in] day        The day of the exception.
 *  @param[in] timerange  The range of the exception.
 */
void timeperiod::add_exception(std::string const& day,
                               std::string const& timerange) {
  _exceptions.push_back(std::make_pair(day, timerange));
}

/**
 *  Add an include relation to this timeperiod.
 *
 *  @param[in] included_tp_id  The id of the included tp.
 */
void timeperiod::add_include_relation(unsigned int included_tp_id) {
  _include.push_back(included_tp_id);
}

/**
 *  Add an exclude relation to this timeperiod.
 *
 *  @param[in] excluded_tp_id  The id of the excluded tp.
 */
void timeperiod::add_exclude_relation(unsigned int excluded_tp_id) {
  _exclude.push_back(excluded_tp_id);
}

/**
 *  Get the timeperiod id.
 *
 *  @return  The timeperiod id.
 */
unsigned int timeperiod::get_id() const {
  return (_id);
}

/**
 *  Get the timeperiod name.
 *
 *  @return  The timeperiod name.
 */
std::string const& timeperiod::get_name() const {
  return (_name);
}

/**
 *  Get the timeperiod alias.
 *
 *  @return  The timeperiod alias.
 */
std::string const& timeperiod::get_alias() const {
  return (_alias);
}

/**
 *  Get the timeperiod sunday.
 *
 *  @return  The timeperiod sunday.
 */
std::string const& timeperiod::get_sunday() const {
  return (_sunday);
}

/**
 *  Get the timeperiod monday.
 *
 *  @return  The timeperiod monday.
 */
std::string const& timeperiod::get_monday() const {
  return (_monday);
}

/**
 *  Get the timeperiod tuesday.
 *
 *  @return  The timeperiod tuesday.
 */
std::string const& timeperiod::get_tuesday() const {
  return (_tuesday);
}

/**
 *  Get the timeperiod wednesday.
 *
 *  @return  The timeperiod wednesday.
 */
std::string const& timeperiod::get_wednesday() const {
  return (_wednesday);
}

/**
 *  Get the timeperiod thursday.
 *
 *  @return  The timeperiod thursday.
 */
std::string const& timeperiod::get_thursday() const {
  return (_thursday);
}

/**
 *  Get the timeperiod friday.
 *
 *  @return  The timeperiod friday.
 */
std::string const& timeperiod::get_friday() const {
  return (_friday);
}

/**
 *  Get the timeperiod saturday.
 *
 *  @return  The timeperiod saturday.
 */
std::string const& timeperiod::get_saturday() const {
  return (_saturday);
}

/**
 *  Get the exceptions.
 *
 *  @return  The exceptions.
 */
std::vector<std::pair<std::string,
                      std::string> > const&
  timeperiod::get_exceptions() const {
  return (_exceptions);
}

/**
 *  Get the included timeperiod.
 *
 *  @return  The included timeperiod.
 */
std::vector<unsigned int> const& timeperiod::get_include() const {
  return (_include);
}

/**
 *  Get the excluded timeperiod.
 *
 *  @return  The excluded timeperiod.
 */
std::vector<unsigned int> const& timeperiod::get_exclude() const {
  return (_exclude);
}
