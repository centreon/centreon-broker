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

#ifndef CCB_EXCEPTIONS_CORRUPTION_HH
#define CCB_EXCEPTIONS_CORRUPTION_HH

#include "com/centreon/broker/exceptions/msg.hh"
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
class corruption : public msg {
 public:
  corruption();
  corruption(corruption const& other);
  ~corruption() throw();
  corruption& operator=(corruption const& other);

  /**
   *  Insert data in message.
   *
   *  @param[in] t Data to insert.
   */
  template <typename T>
  corruption& operator<<(T t) throw() {
    msg::operator<<(t);
    return (*this);
  }
};
}  // namespace exceptions

CCB_END()

#endif  // !CCB_EXCEPTIONS_CORRUPTION_HH
