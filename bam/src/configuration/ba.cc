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

#include "com/centreon/broker/bam/configuration/ba.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] id             BA ID.
 *  @param[in] name           BA name.
 *  @param[in] level          BA level.
 *  @param[in] warning_level  BA warning_level.
 *  @param[in] critical_level BA critical_level.
 */
ba::ba(
      unsigned int id,
      std::string const& name,
      double level,
      double warning_level,
      double critical_level):
  _id(id),
  _name(name),
  _level(level),
  _warning_level(warning_level),
  _critical_level(critical_level)
{}


/**
 *  Copy Constructor
 *
 *  @param[in] other The original object.
 */
ba::ba(ba const& other)
  : _id(other._id),
    _name(other._name),
    _level(other._level),
    _warning_level(other._warning_level),
    _critical_level(other._critical_level)
{}

/**
 *  Assignment Operator.
 *
 *  @param[in] other The original object.
 *
 *  @return this
 */
ba& ba::operator=(ba const& other) {
  if (this != &other) {
    _id = other._id;
    _name =  other._name;
    _level = other._level;
    _warning_level = other._warning_level;
    _critical_level = other._critical_level;
  }
  return (*this);
}

/**
 *  Destructor.
 */
ba::~ba() {}

/**
 *  Get business activity id.
 *
 *  @return An integer representing the value of a business activity.
 */
unsigned int ba::get_id() const {
  return (_id);
}

/**
 *  Get name of the business activity.
 *
 *  @return The name.
 */
std::string const& ba::get_name() const {
  return (_name);
}


/**
 *  Get warning level.
 *
 *  @return The percentage for the warning level.
 */
double ba::get_warning_level() const {
  return (_warning_level);
}

/**
 *  Get critical level.
 *
 *  @return The percentage for the critical level.
 */
double ba::get_critical_level() const {
  return (_critical_level);
}

/**
 *  Set id.
 *
 *  @param[in] id Set business activity id key.
 */
void ba::set_id(unsigned int id) {
  _id = id;
}

/**
 *  Set name.
 *
 *  @param[in] name Name of the BA.
 */
void ba::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Set level.
 *
 *  @param[in] level Level of the BA.
 */
void ba::set_level(double level) {
  _level = level;
}

/**
 *  Set the percentage for warning level
 *
 *  @param[in] warning_level Warning level.
 */
void ba::set_warning_level(double warning_level) {
  _warning_level = warning_level;
}

/**
 *  Set the percentage for criticial level.
 *
 *  @param[in] critical_level Critical level.
 */
void ba::set_critical_level(double critical_level) {
  _critical_level = critical_level;
}



