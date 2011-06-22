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
 *  Get message associated with the exception.
 *
 *  @return Nul-terminated message.
 */
char const* msg::what() const throw () {
  return (misc::stringifier::data());
}
