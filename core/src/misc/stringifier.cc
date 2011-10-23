/*
** Copyright 2009-2011 Merethis
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

#include <stdio.h>
#include <string.h>
#include "com/centreon/broker/misc/stringifier.hh"

using namespace com::centreon::broker::misc;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Perform a checked numeric conversion.
 *
 *  @param[in] t Numeric value to append to the internal buffer.
 *
 *  @return Current instance.
 */
template <typename T>
stringifier& stringifier::_numeric_conversion(char const* format, T t) {
  int length(max_len - _current);
  int retval(snprintf(_buffer + _current, length, format, t));
  if (retval > 0)
    _current += ((retval > length) ? length : retval);
  _buffer[_current] = '\0';
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stringifier::stringifier() throw () {
  reset();
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to build from.
 */
stringifier::stringifier(stringifier const& s) throw () {
  operator=(s);
}

/**
 *  Destructor.
 */
stringifier::~stringifier() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] s Object to copy from.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator=(stringifier const& s) {
  if (this != &s) {
    memcpy(_buffer, s._buffer, (s._current + 1) * sizeof(*_buffer));
    _current = s._current;
  }
  return (*this);
}

/**
 *  Append a boolean to the internal buffer.
 *
 *  @param[in] b Boolean to append, will be expanded to "true" or "false".
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(bool b) throw () {
  operator<<(b ? "true" : "false");
  return (*this);
}

/**
 *  Append a double to the internal buffer.
 *
 *  @param[in] d Double to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(double d) throw () {
  return (_numeric_conversion("%lf", d));
}

/**
 *  Append an integer to the internal buffer.
 *
 *  @param[in] i Integer to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(int i) throw () {
  return (_numeric_conversion("%d", i));
}

/**
 *  Append a long integer to the internal buffer.
 *
 *  @param[in] l Long integer to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(long l) throw () {
  return (_numeric_conversion("%ld", l));
}

/**
 *  Append a long long integer to the internal buffer.
 *
 *  @param[in] ll Long long integer to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(long long ll) throw () {
  return (_numeric_conversion("%lld", ll));
}

/**
 *  @brief Append a QString to the internal buffer.
 *
 *  The QString (Unicode) is converted to latin1 and inserted in the
 *  buffer. However this process might include extra allocation which
 *  can cause exceptions if memory is exhausted. However such errors are
 *  caught and if string could not be converted the string "(unknown)"
 *  is inserted instead.
 *
 *  @param[in] q QString to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(QString const& q) throw () {
  try {
    operator<<(qPrintable(q));
  }
  catch (...) {
    operator<<("(unknown)");
  }
  return (*this);
}

/**
 *  Append an unsigned integer to the internal buffer.
 *
 *  @param[in] i Unsigned integer to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(unsigned int i) throw () {
  return (_numeric_conversion("%u", i));
}

/**
 *  Append an unsigned long to the internal buffer.
 *
 *  @param[in] l Unsigned long to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(unsigned long l) throw () {
  return (_numeric_conversion("%lu", l));
}

/**
 *  Append an unsigned long long to the internal buffer.
 *
 *  @param[in] l Unsigned long long to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(unsigned long long l) throw () {
  return (_numeric_conversion("%llu", l));
}

/**
 *  Append a string to the internal buffer.
 *
 *  @param[in] str String to append.
 *
 *  @return Current instance.
 */
stringifier& stringifier::operator<<(char const* str) throw () {
  size_t len;
  unsigned int remaining;

  // Beware the NULL string.
  if (!str)
    str = "(null)";

  // Check that we won't overlap.
  len = strlen(str);
  remaining = max_len - _current;
  if (remaining < len)
    len = remaining;

  // Append string.
  memcpy(_buffer + _current, str, len * sizeof(*_buffer));
  _current += len;
  _buffer[_current] = '\0';

  return (*this);
}

/**
 *  Get the string buffer.
 *
 *  @return String buffer.
 */
char const* stringifier::data() const throw () {
  return (_buffer);
}

/**
 *  Reset internal buffer to the empty string.
 */
void stringifier::reset() throw () {
  _current = 0;
  _buffer[_current] = '\0';
  return ;
}
