/*
**  Copyright 2010 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include "exceptions/basic.hh"

using namespace exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
basic::basic() throw () {}

/**
 *  Copy constructor.
 *
 *  @param[in] b Object to build from.
 */
basic::basic(basic const& b) throw ()
  : misc::stringifier(b), std::exception(b) {}

/**
 *  Destructor.
 */
basic::~basic() throw () {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] b Object to copy from.
 *
 *  @return Current instance.
 */
basic& basic::operator=(basic const& b) throw ()
{
  misc::stringifier::operator=(b);
  std::exception::operator=(b);
  return (*this);
}

/**
 *  Append a boolean to the exception message.
 *
 *  @param[in] b Boolean to append (expended to "true" or "false").
 *
 *  @return Current instance.
 */
basic& basic::operator<<(bool b) throw ()
{
  stringifier::operator<<(b);
  return (*this);
}

/**
 *  Append a double to the exception message.
 *
 *  @param[in] d Double to append.
 *
 *  @return Current instance.
 */
basic& basic::operator<<(double d) throw ()
{
  stringifier::operator<<(d);
  return (*this);
}

/**
 *  Append an integer to the exception message.
 *
 *  @param[in] i Integer to append.
 *
 *  @return Current instance.
 */
basic& basic::operator<<(int i) throw ()
{
  stringifier::operator<<(i);
  return (*this);
}

/**
 *  Append an unsigned integer to the exception message.
 *
 *  @param[in] i Unsigned integer to append.
 *
 *  @return Current instance.
 */
basic& basic::operator<<(unsigned int i) throw ()
{
  stringifier::operator<<(i);
  return (*this);
}

/**
 *  Append a string to the exception message.
 *
 *  @param[in] str String to append.
 *
 *  @return Current instance.
 */
basic& basic::operator<<(char const* str) throw ()
{
  stringifier::operator<<(str);
  return (*this);
}

/**
 *  Get message associated with the exception.
 *
 *  @return Nul-terminated message.
 */
char const* basic::what() const throw ()
{
  return (_buffer);
}
