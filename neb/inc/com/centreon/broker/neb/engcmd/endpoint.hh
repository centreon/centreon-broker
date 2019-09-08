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

#ifndef CCB_ENGCMD_ENDPOINT_HH
#define CCB_ENGCMD_ENDPOINT_HH

#include <string>
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
namespace engcmd {
/**
 *  @class endpoint endpoint.hh "com/centreon/broker/engcmd/endpoint.hh"
 *  @brief Engine command endpoint.
 *
 *  Build engine command streams.
 */
class endpoint : public io::endpoint {
 public:
  endpoint(std::string const& name, std::string const& command_module_path);
  endpoint(endpoint const& other);
  ~endpoint();
  endpoint& operator=(endpoint const& other);
  std::shared_ptr<io::stream> open();

 private:
  std::string _name;
  std::string _command_module_path;
};
}  // namespace engcmd
}  // namespace neb

CCB_END()

#endif  // !CCB_ENGCMD_FACTORY_HH
