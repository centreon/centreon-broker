/*
** Copyright 2014 Centreon
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

#ifndef CCB_NOTIFICATION_OBJECTS_DEFINES_HH
#define CCB_NOTIFICATION_OBJECTS_DEFINES_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace notification {
namespace objects {
class node_state {
 public:
  enum state_type {
    ok = 0,
    host_up = 0,
    host_down = 1,
    host_unreachable = 2,
    service_ok = 0,
    service_warning = 1,
    service_critical = 2,
    service_unknown = 3
  };

  node_state();
  node_state(state_type type);
  node_state(short);
  node_state(int);
  node_state(node_state const& st);
  node_state& operator=(node_state const& st);
  bool operator==(node_state const& st) const throw();
  bool operator==(node_state::state_type type) const throw();

  operator int() const throw();

  state_type value;
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_OBJECTS_DEFINES_HH
