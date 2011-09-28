/*
** Copyright 2009-2011 Merethis
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

#include <syslog.h>
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker::config;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members to this object.
 *
 *  @param[in] l Object to copy from.
 */
void logger::_internal_copy(logger const& l) {
  _facility = l._facility;
  _level = l._level;
  _name = l._name;
  _type = l._type;
  _types = l._types;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
logger::logger()
  : _facility(LOG_LOCAL0),
    _level(logging::high),
    _type(unknown),
    _types(logging::config_type | logging::error_type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] l Object to copy.
 */
logger::logger(logger const& l) {
  _internal_copy(l);
}

/**
 *  Destructor.
 */
logger::~logger() {}

/**
 *  Assignment operator.
 *
 *  @param[in] l Object to copy.
 *
 *  @return This object.
 */
logger& logger::operator=(logger const& l) {
  _internal_copy(l);
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if objects are equivalent, false otherwise.
 */
bool logger::operator==(logger const& l) const {
  bool ret;
  ret = ((_facility == l._facility)
         && (_level == l._level)
         && (_type == l._type)
         && (_types == l._types));
  if (ret && ((file == _type) || (standard == _type)))
    ret = (ret && (_name == l._name));
  return (ret);
}

/**
 *  Non-equality operator.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if objects are different, false otherwise.
 */
bool logger::operator!=(logger const& l) const {
  return (!(*this == l));
}

/**
 *  Stricly inferior operator.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if current logger is stricly inferior to the argument.
 */
bool logger::operator<(logger const& l) const {
  bool ret;
  if (_facility != l._facility)
    ret = (_facility < l._facility);
  else if (_level != l._level)
    ret = (_level < l._level);
  else if (_type != l._type)
    ret = (_type < l._type);
  else if (_types != l._types)
    ret = (_types < l._types);
  else if (((file == _type) || (standard == _type))
           && (_name != l._name))
    ret = (_name < l._name);
  else
    ret = false;
  return (ret);
}

/**
 *  Set the config parameter.
 *
 *  @param[in] c New value.
 */
void logger::config(bool c) {
  if (c)
    _types = (_types | logging::config_type);
  else
    _types = (_types & ~logging::config_type);
  return ;
}

/**
 *  Get the config parameter.
 *
 *  @return Current value.
 */
bool logger::config() const {
  return (_types & logging::config_type);
}

/**
 *  Set the debug parameter.
 *
 *  @param[in] d New value.
 */
void logger::debug(bool d) {
  if (d)
    _types = (_types | logging::debug_type);
  else
    _types = (_types & ~logging::debug_type);
  return ;
}

/**
 *  Get the debug parameter.
 *
 *  @return Current value.
 */
bool logger::debug() const {
  return (_types & logging::debug_type);
}

/**
 *  Set the error parameter.
 *
 *  @param[in] e New value.
 */
void logger::error(bool e) {
  if (e)
    _types = (_types | logging::error_type);
  else
    _types = (_types & ~logging::error_type);
  return ;
}

/**
 *  Get the error parameter.
 *
 *  @return Current value.
 */
bool logger::error() const {
  return (_types & logging::error_type);
}

/**
 *  Set the facility parameter.
 *
 *  @param[in] f Facility.
 */
void logger::facility(int f) {
  _facility = f;
  return ;
}

/**
 *  Get the facility parameter.
 *
 *  @return Current value.
 */
int logger::facility() const {
  return (_facility);
}

/**
 *  Set the info parameter.
 *
 *  @param[in] i New value.
 */
void logger::info(bool i) {
  if (i)
    _types = (_types | logging::info_type);
  else
    _types = (_types & ~logging::info_type);
  return ;
}

/**
 *  Get the info parameter.
 *
 *  @return Current value.
 */
bool logger::info() const {
  return (_types & logging::info_type);
}

/**
 *  Set the level parameter.
 *
 *  @param[in] l New value.
 */
void logger::level(logging::level l) {
  _level = l;
  return ;
}

/**
 *  Get the level parameter.
 *
 *  @return Current value.
 */
com::centreon::broker::logging::level logger::level() const {
  return (_level);
}

/**
 *  Set the name parameter.
 *
 *  @param[in] n New value.
 */
void logger::name(QString const& n) {
  _name = n;
  return ;
}

/**
 *  Get the name parameter.
 *
 *  @return Current value.
 */
QString const& logger::name() const {
  return (_name);
}

/**
 *  Set the type parameter.
 *
 *  @param[in] lt New value.
 */
void logger::type(logger::logger_type lt) {
  _type = lt;
  return ;
}

/**
 *  Get the type parameter.
 *
 *  @return Current value.
 */
logger::logger_type logger::type() const {
  return (_type);
}

/**
 *  Set the types parameter.
 *
 *  @param[in] t New value.
 */
void logger::types(unsigned int t) {
  _types = t;
  return ;
}

/**
 *  Get the types parameter.
 *
 *  @return Current value.
 */
unsigned int logger::types() const {
  return (_types);
}
