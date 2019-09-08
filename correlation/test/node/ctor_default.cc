/*
** Copyright 2011 Centreon
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

#include "com/centreon/broker/correlation/node.hh"

using namespace com::centreon::broker;

/**
 *  Check that node is properly default constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  correlation::node n;

  // Check default construction.
  return (!n.get_children().empty() || !n.get_dependeds().empty() ||
          !n.get_dependencies().empty() || (n.host_id != 0) || n.in_downtime ||
          n.my_issue.get() || !n.get_parents().empty() || (n.service_id != 0) ||
          (n.current_state != 0));
}
