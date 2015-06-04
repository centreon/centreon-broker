/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/exceptions/timeout.hh"

using namespace com::centreon::broker::exceptions;

/**
 *  Default constructor.
 */
timeout::timeout() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to build from.
 */
timeout::timeout(timeout const& other)
  : misc::stringifier(other), std::exception(other) {}

/**
 *  Destructor.
 */
timeout::~timeout() throw () {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] other  Object to copy from.
 *
 *  @return This instance.
 */
timeout& timeout::operator=(timeout const& other) {
  if (this != &other) {
    misc::stringifier::operator=(other);
    std::exception::operator=(other);
  }
  return (*this);
}

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
timeout* timeout::clone() const {
  return (new timeout(*this));
}

/**
 *  Rethrow the exception.
 */
void timeout::rethrow() const {
  throw (*this);
  return ;
}

/**
 *  Get message associated with the exception.
 *
 *  @return Nul-terminated message.
 */
char const* timeout::what() const throw () {
  return (misc::stringifier::data());
}
