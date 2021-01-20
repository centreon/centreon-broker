/*
** Copyright 2009-2011,2017,2020 Centreon
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

#ifndef CCB_EXCEPTIONS_CORRUPTION_HH
#define CCB_EXCEPTIONS_CORRUPTION_HH

#include "com/centreon/exceptions/msg_fmt.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class corruption corruption.hh
 * "com/centreon/broker/exceptions/corruption.hh"
 *  @brief Shutdown exception class.
 *
 *  This exception is thrown when someone attemps to read from a
 *  stream that has been corruption.
 */
class corruption : public com::centreon::exceptions::msg_fmt {
 public:
  template <typename... Args>
  explicit corruption(std::string const& str, const Args&... args)
    : msg_fmt(str, args...) {}

  corruption() = delete;
  ~corruption() noexcept {}
  corruption& operator=(const corruption&) = delete;
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_CORRUPTION_HH
