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

#ifndef CCB_GENERATOR_ENDPOINT_HH
#define CCB_GENERATOR_ENDPOINT_HH

#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace generator {
/**
 *  @class endpoint endpoint.hh "com/centreon/broker/generator/endpoint.hh"
 *  @brief Create generator stream.
 *
 *  Create a generator stream.
 */
class endpoint : public io::endpoint {
 public:
  enum endpoint_type { type_receiver = 1, type_sender };

  endpoint(endpoint_type type);
  ~endpoint();
  std::shared_ptr<io::stream> open();

 private:
  endpoint(endpoint const& other);
  endpoint& operator=(endpoint const& other);

  endpoint_type _type;
};
}  // namespace generator

CCB_END()

#endif  // !CCB_GENERATOR_ENDPOINT_HH
