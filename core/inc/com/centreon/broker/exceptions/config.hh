/*
** Copyright 2015 Centreon
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

#ifndef CCB_EXCEPTIONS_CONFIG_HH
#define CCB_EXCEPTIONS_CONFIG_HH

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace exceptions {
/**
 *  @class config config.hh "com/centreon/broker/exceptions/config.hh"
 *  @brief Configuration exception.
 *
 *  Such exceptions are thrown in case of configuration errors.
 */
class config : public msg {
 public:
  config();
  config(config const& other);
  ~config() throw();
  config& operator=(config const& other);

  /**
   *  Insert data in message.
   *
   *  @param[in] t  Data to insert.
   *
   *  @return This object.
   */
  template <typename T>
  config& operator<<(T t) {
    msg::operator<<(t);
    return (*this);
  }
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_CONFIG_HH
