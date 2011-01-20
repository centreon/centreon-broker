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

#include <pthread.h>
#include <stdio.h>
#include "exceptions/concurrency.hh"

using namespace exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
concurrency::concurrency(int r, pthread_t id)
  : retval(r), _thread_id(id) {
  char buff[64];
  if (snprintf(buff, sizeof(buff), "[%lx] ", (unsigned long)_thread_id)
      > 0)
    operator<<(buff);
}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
concurrency::concurrency(concurrency const& c) throw ()
  : retval(c), _thread_id(c._thread_id) {}

/**
 *  Destructor.
 */
concurrency::~concurrency() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
concurrency& concurrency::operator=(concurrency const& c) throw () {
  retval::operator=(c);
  _thread_id = c._thread_id;
  return (*this);
}

/**
 *  Append a boolean to the exception message.
 *
 *  @param[in] b Boolean to append.
 *
 *  @return This instance.
 */
concurrency& concurrency::operator<<(bool b) throw () {
  retval::operator<<(b);
  return (*this);
}

/**
 *  Append a double to the exception message.
 *
 *  @param[in] d Double to append.
 *
 *  @return This instance.
 */
concurrency& concurrency::operator<<(double d) throw () {
  retval::operator<<(d);
  return (*this);
}

/**
 *  Append an integer to the exception message.
 *
 *  @param[in] i Integer to append.
 *
 *  @return This instance.
 */
concurrency& concurrency::operator<<(int i) throw () {
  retval::operator<<(i);
  return (*this);
}

/**
 *  Append an unsigned integer to the exception message.
 *
 *  @param[in] i Unsigned integer to append.
 *
 *  @return This instance.
 */
concurrency& concurrency::operator<<(unsigned int i) throw () {
  retval::operator<<(i);
  return (*this);
}

/**
 *  Append a string to the exception message.
 *
 *  @param[in] str String to append.
 *
 *  @return This instance.
 */
concurrency& concurrency::operator<<(char const* str) throw () {
  retval::operator<<(str);
  return (*this);
}

/**
 *  Append a timestamp to the exception message.
 *
 *  @param[in] t Timestamp to append.
 *
 *  @return This instance.
 */
concurrency& concurrency::operator<<(time_t t) throw () {
  retval::operator<<(t);
  return (*this);
}

/**
 *  Get the thread ID associated with this exception.
 *
 *  @return Thread ID.
 */
pthread_t concurrency::get_thread_id() const throw () {
  return (_thread_id);
}
