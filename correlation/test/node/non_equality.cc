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

#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/node.hh"

using namespace com::centreon::broker;

/**
 *  Check that node can be properly assigned.
 *
 *  @return 0 on success.
 */
int main() {
  // Base object.
  correlation::node bn;
  bn.host_id = 42;
  bn.in_downtime = true;
  bn.my_issue.reset(new correlation::issue);
  bn.my_issue->end_time = 234;
  bn.my_issue->start_time = 7678353;
  bn.service_id = 765334;
  bn.current_state = 2;

  // Linked objects.
  correlation::node n1;
  correlation::node n2;
  correlation::node n3;
  correlation::node n4;
  bn.add_child(&n1);
  bn.add_depended(&n2);
  bn.add_dependency(&n3);
  bn.add_parent(&n4);

  // Copy.
  correlation::node cn(bn);

  // Reset base object.
  correlation::node dn(bn);
  dn.host_id = 23;
  dn.in_downtime = false;
  dn.my_issue.reset();
  dn.service_id = 2347;
  dn.current_state = 1;
  dn.remove_child(&n1);
  dn.remove_dependency(&n3);

  // Check equality.
  return ((bn != cn) || !(bn != dn) || !(cn != dn) || (bn != bn) ||
          (cn != cn) || (dn != dn));
}
