/*
** Copyright 2009-2011,2017 Centreon
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
msg::msg() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to build from.
 */
msg::msg(msg const& other) : misc::stringifier(other), std::exception(other) {}

/**
 *  Destructor.
 */
msg::~msg() throw() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] other  Object to copy from.
 *
 *  @return This instance.
 */
msg& msg::operator=(msg const& other) {
  misc::stringifier::operator=(other);
  std::exception::operator=(other);
  return (*this);
}

/**
 *  Get message associated with the exception.
 *
 *  @return Nul-terminated message.
 */
char const* msg::what() const throw() {
  return (misc::stringifier::data());
}
