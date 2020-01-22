/*
** Copyright 2017-2018 Centreon
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

#ifndef CCB_LUA_FACTORY_HH
#define CCB_LUA_FACTORY_HH

#include "com/centreon/broker/io/factory.hh"

CCB_BEGIN()

namespace lua {
/**
 *  @class factory factory.hh "com/centreon/broker/lua/factory.hh"
 *  @brief lua  layer factory.
 *
 *  Build lua layer objects.
 */
class factory : public io::factory {
 public:
  factory() = default;
  factory(factory const& other) = delete;
  ~factory() = default;
  factory& operator=(factory const& other) = delete;
  bool has_endpoint(config::endpoint& cfg) const;
  io::endpoint* new_endpoint(config::endpoint& cfg,
                             bool& is_acceptor,
                             std::shared_ptr<persistent_cache> cache =
                                 std::shared_ptr<persistent_cache>()) const;
};
}  // namespace lua

CCB_END()

#endif  // !CCB_LUA_FACTORY_HH
