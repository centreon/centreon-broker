/*
** Copyright 2015,2017,2020 Centreon
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

#ifndef CCB_EXCEPTIONS_TIMEOUT_HH
#define CCB_EXCEPTIONS_TIMEOUT_HH

#include <exception>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class timeout timeout.hh "com/centreon/broker/exceptions/timeout.hh"
 *  @brief Timeout exception.
 *
 *  Exception that is thrown upon timeout.
 */
class timeout : public std::exception {
 public:
  timeout() noexcept : std::exception() {}
  timeout& operator=(const timeout&) = delete;
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_TIMEOUT_HH
