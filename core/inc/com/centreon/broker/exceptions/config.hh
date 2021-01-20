/*
** Copyright 2015,2020 Centreon
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

#ifndef CC_EXCEPTIONS_CONFIG_HH
#define CC_EXCEPTIONS_CONFIG_HH

#include "com/centreon/exceptions/msg_fmt.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class config config.hh "com/centreon/broker/exceptions/config.hh"
 *  @brief Configuration exception.
 *
 *  Such exceptions are thrown in case of configuration errors.
 */
class config : public com::centreon::exceptions::msg_fmt {
 public:
  template <typename...Args>
  explicit config(std::string const& str, const Args&... args)
    : msg_fmt(str, args...) {}
  config() = delete;
  ~config() noexcept {}
  config& operator=(const config&) = delete;
};
}  // namespace exceptions

CCB_END()

#endif  // !CC_EXCEPTIONS_CONFIG_HH
