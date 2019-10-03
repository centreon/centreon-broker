/*
** Copyright 2017 Centreon
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

#ifndef CCB_GENERATOR_SENDER_HH
#define CCB_GENERATOR_SENDER_HH

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace generator {
/**
 *  @class sender sender.hh "com/centreon/broker/generator/sender.hh"
 *  @brief Generate events.
 *
 *  Generate events.
 */
class sender : public io::stream {
 public:
  sender();
  ~sender();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  int write(std::shared_ptr<io::data> const& d);

 private:
  sender(sender const& other);
  sender& operator=(sender const& other);

  uint32_t _number;
};
}  // namespace generator

CCB_END()

#endif  // !CCB_GENERATOR_SENDER_HH
