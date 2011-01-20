/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "logging/internal.hh"
#include "logging/temp_logger.hh"

using namespace logging;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] t Object to copy from.
 */
void temp_logger::_internal_copy(temp_logger const& t) {
  _level = t._level;
  _type = t._type;
  _copied = false;
  t._copied = true;
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
temp_logger::temp_logger(type log_type)
  : _copied(false), _level(MEDIUM), _type(log_type) {}

/**
 *  Copy constructor.
 *
 *  @param[in] t Object to copy.
 */
temp_logger::temp_logger(temp_logger const& t) : misc::stringifier(t) {
  _internal_copy(t);
}

/**
 *  Destructor.
 */
temp_logger::~temp_logger() {
  if (!_copied) {
    operator<<("\n");
    for (std::map<backend*, std::pair<unsigned int, level> >::iterator
           it = backends.begin(), end = backends.end();
         it != end;
         ++it)
      if ((it->second.first & _type) && (it->second.second >= _level))
        it->first->log_msg(_buffer, _current, _type, _level);
  }
}

/**
 *  Assignment operator.
 *
 *  @param[in] t Object to copy.
 *
 *  @return This object.
 */
temp_logger& temp_logger::operator=(temp_logger const& t) {
  misc::stringifier::operator=(t);
  _internal_copy(t);
  return (*this);
}

/**
 *  Log a boolean.
 *
 *  @param[in] b Boolean to log (expanded to "true" or "false").
 *
 *  @return Current instance.
 */
temp_logger& temp_logger::operator<<(bool b) throw () {
  stringifier::operator<<(b);
  return (*this);
}

/**
 *  Log a double.
 *
 *  @param[in] d Double to log.
 *
 *  @return Current instance.
 */
temp_logger& temp_logger::operator<<(double d) throw () {
  stringifier::operator<<(d);
  return (*this);
}

/**
 *  Log an integer.
 *
 *  @param[in] i Integer to log.
 *
 *  @return Current instance.
 */
temp_logger& temp_logger::operator<<(int i) throw () {
  stringifier::operator<<(i);
  return (*this);
}

/**
 *  Log an unsigned integer.
 *
 *  @param[in] i Unsigned integer to log.
 *
 *  @return Current instance.
 */
temp_logger& temp_logger::operator<<(unsigned int i) throw () {
  stringifier::operator<<(i);
  return (*this);
}

/**
 *  Log a string.
 *
 *  @param[in] str String to log.
 *
 *  @return Current instance.
 */
temp_logger& temp_logger::operator<<(char const* str) throw () {
  stringifier::operator<<(str);
  return (*this);
}

/**
 *  Change log level.
 *
 *  @param[in] l New level.
 *
 *  @return Current instance.
 */
temp_logger& temp_logger::operator<<(level l) throw () {
  _level = l;
  return (*this);
}
