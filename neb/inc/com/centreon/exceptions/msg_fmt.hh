/*
** Copyright 2020 Centreon
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
#ifndef CC_EXCEPTIONS_MSG_FMT_HH
#define CC_EXCEPTIONS_MSG_FMT_HH

#include <fmt/format.h>
#include <exception>
#include <string>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace exceptions {
/**
 *  @class msg_fmt msg_fmt.hh "com/centreon/exceptions/msg_fmt.hh"
 *  @brief Base exception class.
 *
 *  Simple exception class containing an basic error message.
 */

class msg_fmt : public std::exception {
  const std::string _msg;

 public:
  msg_fmt() = delete;

  template <typename... Args>
  explicit msg_fmt(std::string const& str, const Args&... args)
      : _msg(fmt::format(str, args...)) {}

  virtual msg_fmt& operator=(msg_fmt const&) = delete;
  virtual char const* what() const noexcept { return _msg.c_str(); }
};
}  // namespace exceptions
CC_END()
#endif  // !CC_EXCEPTIONS_MSG_FMT_HH
