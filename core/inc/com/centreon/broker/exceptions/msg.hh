/*
** Copyright 2009-2011,2015,2017 Centreon
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

#ifndef CCB_EXCEPTIONS_MSG_HH_
#define CCB_EXCEPTIONS_MSG_HH_

#include <exception>
#include "com/centreon/broker/misc/stringifier.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class msg msg.hh "com/centreon/broker/exceptions/msg.hh"
 *  @brief Base class of exceptions thrown in Centreon Broker.
 *
 *  msg is a simple exception class that is only associated with a
 *  message. msg accepts multiple types on input.
 */
class msg : protected misc::stringifier, public std::exception {
 public:
  msg() = default;
  msg(msg const& other) : misc::stringifier(other), std::exception(other) {}
  virtual ~msg() noexcept {}
  msg& operator=(msg const&) = delete;
  virtual char const* what() const noexcept { return misc::stringifier::data(); }

  /**
   *  Insert data in message.
   *
   *  @param[in] t Data to insert.
   */
  template <typename T>
  msg& operator<<(T t) {
    *(misc::stringifier*)this << t;
    return *this;
  }
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_MSG_HH
