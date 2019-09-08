/*
** Copyright 2011-2012 Centreon
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

#include <iostream>

/**
 *  Check that node is properly copy constructed.
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
  n1.host_id = 1;
  n1.service_id = 1;
  correlation::node n2;
  n2.host_id = 1;
  n2.service_id = 2;
  correlation::node n3;
  n3.host_id = 1;
  n3.service_id = 3;
  correlation::node n4;
  n4.host_id = 1;
  n4.service_id = 4;
  bn.add_child(&n1);
  bn.add_depended(&n2);
  bn.add_dependency(&n3);
  bn.add_parent(&n4);

  // Copy.
  correlation::node cn(bn);

  // Reset base object.
  bn.remove_child(&n1);
  bn.remove_dependency(&n3);
  bn.host_id = 23;
  bn.in_downtime = false;
  bn.my_issue.reset();
  bn.service_id = 2347;
  bn.current_state = 1;

  // Check copy construction.
  return (
      (bn.host_id != 23) || bn.in_downtime || bn.my_issue.get() ||
      (bn.service_id != 2347) || (bn.current_state != 1) ||
      !bn.get_children().empty() || (bn.get_dependeds().size() != 1) ||
      !bn.get_dependencies().empty() || (bn.get_parents().size() != 1) ||
      (cn.host_id != 42) || !cn.in_downtime || !cn.my_issue.get() ||
      (cn.my_issue->end_time != 234) || (cn.my_issue->start_time != 7678353) ||
      (cn.service_id != 765334) || (cn.current_state != 2) ||
      (cn.get_children().size() != 1) || (*cn.get_children().begin() != &n1) ||
      (cn.get_dependeds().size() != 1) ||
      (*cn.get_dependeds().begin() != &n2) ||
      (cn.get_dependencies().size() != 1) ||
      (*cn.get_dependencies().begin() != &n3) ||
      (cn.get_parents().size() != 1) || (*cn.get_parents().begin() != &n4) ||
      (n1.get_parents().size() != 1) || (n2.get_dependencies().size() != 2) ||
      (n3.get_dependeds().size() != 1) || (n4.get_children().size() != 2));
}
