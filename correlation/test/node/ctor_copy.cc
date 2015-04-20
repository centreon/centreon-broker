/*
** Copyright 2011-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
  bn.state = 2;

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
  bn.state = 1;

  // Check copy construction.
  return ((bn.host_id != 23)
          || bn.in_downtime
          || bn.my_issue.get()
          || (bn.service_id != 2347)
          || (bn.state != 1)
          || !bn.get_children().isEmpty()
          || (bn.get_dependeds().size() != 1)
          || !bn.get_dependencies().isEmpty()
          || (bn.get_parents().size() != 1)
          || (cn.host_id != 42)
          || !cn.in_downtime
          || !cn.my_issue.get()
          || (cn.my_issue->end_time != 234)
          || (cn.my_issue->start_time != 7678353)
          || (cn.service_id != 765334)
          || (cn.state != 2)
          || (cn.get_children().size() != 1)
          || (*cn.get_children().begin() != &n1)
          || (cn.get_dependeds().size() != 1)
          || (*cn.get_dependeds().begin() != &n2)
          || (cn.get_dependencies().size() != 1)
          || (*cn.get_dependencies().begin() != &n3)
          || (cn.get_parents().size() != 1)
          || (*cn.get_parents().begin() != &n4)
          || (n1.get_parents().size () != 1)
          || (n2.get_dependencies().size() != 2)
          || (n3.get_dependeds().size() != 1)
          || (n4.get_children().size() != 2));
}
