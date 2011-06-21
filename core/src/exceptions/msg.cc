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

#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
msg::msg() throw () {}

/**
 *  Copy constructor.
 *
 *  @param[in] b Object to build from.
 */
msg::msg(msg const& b) throw ()
  : misc::stringifier(b), std::exception(b) {}

/**
 *  Destructor.
 */
msg::~msg() throw () {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] b Object to copy from.
 *
 *  @return This instance.
 */
msg& msg::operator=(msg const& b) throw () {
  misc::stringifier::operator=(b);
  std::exception::operator=(b);
  return (*this);
}

/**
 *  Append a boolean to the exception message.
 *
 *  @param[in] b Boolean to append (expended to "true" or "false").
 *
 *  @return This instance.
 */
msg& msg::operator<<(bool b) throw () {
  stringifier::operator<<(b);
  return (*this);
}

/**
 *  Append a double to the exception message.
 *
 *  @param[in] d Double to append.
 *
 *  @return This instance.
 */
msg& msg::operator<<(double d) throw () {
  stringifier::operator<<(d);
  return (*this);
}

/**
 *  Append an integer to the exception message.
 *
 *  @param[in] i Integer to append.
 *
 *  @return This instance.
 */
msg& msg::operator<<(int i) throw () {
  stringifier::operator<<(i);
  return (*this);
}

/**
 *  Append an unsigned integer to the exception message.
 *
 *  @param[in] i Unsigned integer to append.
 *
 *  @return This instance.
 */
msg& msg::operator<<(unsigned int i) throw () {
  stringifier::operator<<(i);
  return (*this);
}

/**
 *  Append an unsigned long long to the exceptions message.
 *
 *  @param[in] l Unsigned long long to append.
 *
 *  @return This instance.
 */
msg& msg::operator<<(unsigned long long l) throw () {
  stringifier::operator<<(l);
  return (*this);
}

/**
 *  Append a string to the exception message.
 *
 *  @param[in] str String to append.
 *
 *  @return This instance.
 */
msg& msg::operator<<(char const* str) throw () {
  stringifier::operator<<(str);
  return (*this);
}

/**
 *  Append a timestamp to the exception message.
 *
 *  @param[in] t Timestamp to append.
 *
 *  @return This instance.
 */
msg& msg::operator<<(time_t t) throw () {
  stringifier::operator<<(t);
  return (*this);
}

/**
 *  Get message associated with the exception.
 *
 *  @return Nul-terminated message.
 */
char const* msg::what() const throw () {
  return (_buffer);
}
