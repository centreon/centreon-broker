/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_MACRO_GENERATOR_HH
#define CCB_NOTIFICATION_MACRO_GENERATOR_HH

#include <QHash>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/macro_context.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/state.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class macro_generator macro_generator.hh
 * "com/centreon/broker/notification/macro_generator.hh"
 *  @brief Generates the macros.
 */
class macro_generator {
 public:
  typedef QHash<std::string, std::string> macro_container;
  macro_generator();

  void generate(macro_container& container,
                objects::node_id id,
                objects::contact const& cnt,
                state const& st,
                node_cache const& cache,
                action const& act) const;

 private:
  static bool _get_global_macros(std::string const& macro_name,
                                 state const& st,
                                 std::string& result);
  bool _get_x_macros(std::string const& macro_name,
                     macro_context const& context,
                     std::string& result) const;
  static bool _get_custom_macros(std::string const& macro_name,
                                 objects::node_id id,
                                 node_cache const& cache,
                                 std::string& result);

  typedef QHash<std::string, std::string (*)(macro_context const&)> x_macro_map;
  static x_macro_map _map;
  static void _fill_x_macro_map(x_macro_map& map);
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_MACRO_GENERATOR_HH
