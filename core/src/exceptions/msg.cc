/*
** Copyright 2009-2011 Centreon
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
msg::msg()  {}

/**
 *  Copy constructor.
 *
 *  @param[in] b Object to build from.
 */
msg::msg(msg const& b)
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
msg& msg::operator=(msg const& b)  {
  misc::stringifier::operator=(b);
  std::exception::operator=(b);
  return (*this);
}

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
msg* msg::clone() const {
  return (new msg(*this));
}

/**
 *  Rethrow the exception.
 */
void msg::rethrow() const {
  throw (*this);
  return ;
}

/**
 *  Get message associated with the exception.
 *
 *  @return Nul-terminated message.
 */
char const* msg::what() const throw () {
  return (misc::stringifier::data());
}
