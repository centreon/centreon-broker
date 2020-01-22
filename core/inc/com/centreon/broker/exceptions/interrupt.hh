/*
** Copyright 2017 - 2020 Centreon
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

#ifndef CCB_EXCEPTIONS_INTERRUPT_HH
#define CCB_EXCEPTIONS_INTERRUPT_HH

#include <exception>
#include "com/centreon/broker/misc/stringifier.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class interrupt interrupt.hh "com/centreon/broker/exceptions/interrupt.hh"
 *  @brief Interrupt exception.
 *
 *  Exception that is thrown upon interrupted action.
 */
class interrupt : protected misc::stringifier, public std::exception {
 public:
  interrupt() = default;
  interrupt(interrupt const& other)
      : misc::stringifier(other), std::exception(other) {}
  virtual ~interrupt() noexcept {}
  interrupt& operator=(const interrupt&) = delete;
  virtual char const* what() const noexcept { return misc::stringifier::data(); }

  /**
   *  Insert data in message.
   *
   *  @param[in] t  Data to insert.
   */
  template <typename T>
  interrupt& operator<<(T t) noexcept {
    *(misc::stringifier*)this << t;
    return *this;
  }
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_INTERRUPT_HH
