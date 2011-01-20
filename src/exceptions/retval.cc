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

#include "exceptions/retval.hh"

using namespace exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
retval::retval(int r) throw () : _retval(r) {}

/**
 *  Copy constructor.
 *
 *  @param[in] r Object to build from.
 */
retval::retval(retval const& r) throw () : basic(r) {}

/**
 *  Destructor.
 */
retval::~retval() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] r Object to copy from.
 *
 *  @return This instance.
 */
retval& retval::operator=(retval const& r) throw () {
  basic::operator=(r);
  _retval = r._retval;
  return (*this);
}

/**
 *  Append a boolean to the exception message.
 *
 *  @param[in] b Boolean to append.
 *
 *  @return This instance.
 */
retval& retval::operator<<(bool b) throw () {
  basic::operator<<(b);
  return (*this);
}

/**
 *  Append a double to the exception message.
 *
 *  @param[in] d Double to append.
 *
 *  @return This instance.
 */
retval& retval::operator<<(double d) throw () {
  basic::operator<<(d);
  return (*this);
}

/**
 *  Append an integer to the exception message.
 *
 *  @param[in] i Integer to append.
 *
 *  @return This instance.
 */
retval& retval::operator<<(int i) throw () {
  basic::operator<<(i);
  return (*this);
}

/**
 *  Append an unsigned integer to the exception message.
 *
 *  @param[in] i Unsigned integer to append.
 *
 *  @return This instance.
 */
retval& retval::operator<<(unsigned int i) throw () {
  basic::operator<<(i);
  return (*this);
}

/**
 *  Append a string to the exception message.
 *
 *  @param[in] str String to append.
 *
 *  @return This instance.
 */
retval& retval::operator<<(char const* str) throw () {
  basic::operator<<(str);
  return (*this);
}

/**
 *  Get the return value.
 *
 *  @return Return value.
 */
int retval::get_retval() const throw () {
  return (_retval);
}

/**
 *  Set the return value.
 *
 *  @param[in] r New return value.
 */
void retval::set_retval(int r) throw () {
  _retval = r;
  return ;
}
