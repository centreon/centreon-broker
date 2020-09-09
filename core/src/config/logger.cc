/*
** Copyright 2009-2012,2017 Centreon
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

#include "com/centreon/broker/config/logger.hh"

#include <syslog.h>

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker::config;

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
      _max_size(10000000000ull),
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
  return *this;
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
  ret = ((_facility == l._facility) && (_level == l._level) &&
         (_max_size == l._max_size) && (_type == l._type) &&
         (_types == l._types));
  if (ret && ((file == _type) || (standard == _type)))
    ret = (ret && (_name == l._name));
  return ret;
}

/**
 *  Non-equality operator.
 *
 *  @param[in] l Logger to compare to.
 *
 *  @return true if objects are different, false otherwise.
 */
bool logger::operator!=(logger const& l) const {
  return !(*this == l);
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
  else if (_max_size != l._max_size)
    ret = (_max_size < l._max_size);
  else if (_type != l._type)
    ret = (_type < l._type);
  else if (_types != l._types)
    ret = (_types < l._types);
  else if (((file == _type) || (standard == _type)) && (_name != l._name))
    ret = (_name < l._name);
  else
    ret = false;
  return ret;
}

/**
 *  Set the config parameter.
 *
 *  @param[in] c New value.
 */
void logger::config(bool c) throw() {
  if (c)
    _types = (_types | logging::config_type);
  else
    _types = (_types & ~logging::config_type);
}

/**
 *  Get the config parameter.
 *
 *  @return Current value.
 */
bool logger::config() const throw() {
  return _types & logging::config_type;
}

/**
 *  Set the debug parameter.
 *
 *  @param[in] d New value.
 */
void logger::debug(bool d) throw() {
  if (d)
    _types = (_types | logging::debug_type);
  else
    _types = (_types & ~logging::debug_type);
}

/**
 *  Get the debug parameter.
 *
 *  @return Current value.
 */
bool logger::debug() const throw() {
  return _types & logging::debug_type;
}

/**
 *  Set the error parameter.
 *
 *  @param[in] e New value.
 */
void logger::error(bool e) throw() {
  if (e)
    _types = (_types | logging::error_type);
  else
    _types = (_types & ~logging::error_type);
}

/**
 *  Get the error parameter.
 *
 *  @return Current value.
 */
bool logger::error() const throw() {
  return _types & logging::error_type;
}

/**
 *  Set the facility parameter.
 *
 *  @param[in] f Facility.
 */
void logger::facility(int f) throw() {
  _facility = f;
}

/**
 *  Get the facility parameter.
 *
 *  @return Current value.
 */
int logger::facility() const throw() {
  return _facility;
}

/**
 *  Set the info parameter.
 *
 *  @param[in] i New value.
 */
void logger::info(bool i) throw() {
  if (i)
    _types = (_types | logging::info_type);
  else
    _types = (_types & ~logging::info_type);
}

/**
 *  Get the info parameter.
 *
 *  @return Current value.
 */
bool logger::info() const throw() {
  return _types & logging::info_type;
}

/**
 *  Set the perf parameter.
 *
 *  @param[in] i New value.
 */
void logger::perf(bool p) throw() {
  if (p)
    _types = (_types | logging::perf_type);
  else
    _types = (_types & ~logging::perf_type);
}

/**
 *  Get the perf parameter.
 *
 *  @return Current value.
 */
bool logger::perf() const throw() {
  return _types & logging::perf_type;
}

/**
 *  Set the level parameter.
 *
 *  @param[in] l New value.
 */
void logger::level(logging::level l) throw() {
  _level = l;
}

/**
 *  Get the level parameter.
 *
 *  @return Current value.
 */
com::centreon::broker::logging::level logger::level() const throw() {
  return _level;
}

/**
 *  Set the maximum file size.
 *
 *  @param[in] max Maximum file size in bytes.
 */
void logger::max_size(unsigned long long max) throw() {
  _max_size = max;
}

/**
 *  Get the maximum file size.
 *
 *  @return Maximum file size.
 */
unsigned long long logger::max_size() const throw() {
  return _max_size;
}

/**
 *  Set the name parameter.
 *
 *  @param[in] n New value.
 */
void logger::name(std::string const& n) {
  _name = n;
}

/**
 *  Get the name parameter.
 *
 *  @return Current value.
 */
std::string const& logger::name() const throw() {
  return _name;
}

/**
 *  Set the type parameter.
 *
 *  @param[in] lt New value.
 */
void logger::type(logger::logger_type lt) throw() {
  _type = lt;
}

/**
 *  Get the type parameter.
 *
 *  @return Current value.
 */
logger::logger_type logger::type() const throw() {
  return _type;
}

/**
 *  Set the types parameter.
 *
 *  @param[in] t New value.
 */
void logger::types(uint32_t t) throw() {
  _types = t;
}

/**
 *  Get the types parameter.
 *
 *  @return Current value.
 */
uint32_t logger::types() const throw() {
  return _types;
}

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
  _max_size = l._max_size;
  _name = l._name;
  _type = l._type;
  _types = l._types;
}
