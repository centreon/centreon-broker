/*
** Copyright 2011 Merethis
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
  bn.since = 3945239074u;
  bn.state = 2;

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
  correlation::node cn;
  cn = bn;

  // Reset base object.
  bn.host_id = 23;
  bn.in_downtime = false;
  bn.my_issue.reset();
  bn.service_id = 2347;
  bn.since = 553445;
  bn.state = 1;
  bn.remove_child(&n1);
  bn.remove_dependency(&n3);

  // Check copy construction.
  return ((bn.host_id != 23)
          || bn.in_downtime
          || bn.my_issue.get()
          || (bn.service_id != 2347)
          || (bn.since != 553445)
          || (bn.state != 1)
          || !bn.children().isEmpty()
          || (bn.depended_by().size() != 1)
          || !bn.depends_on().isEmpty()
          || (bn.parents().size() != 1)
          || (cn.host_id != 42)
          || !cn.in_downtime
          || !cn.my_issue.get()
          || (cn.my_issue->end_time != 234)
          || (cn.my_issue->start_time != 7678353)
          || (cn.service_id != 765334)
          || (cn.since != static_cast<time_t>(3945239074u))
          || (cn.state != 2)
          || (cn.children().size() != 1)
          || (*cn.children().begin() != &n1)
          || (cn.depended_by().size() != 1)
          || (*cn.depended_by().begin() != &n2)
          || (cn.depends_on().size() != 1)
          || (*cn.depends_on().begin() != &n3)
          || (cn.parents().size() != 1)
          || (*cn.parents().begin() != &n4)
          || (n1.parents().size () != 1)
          || (n2.depends_on().size() != 2)
          || (n3.depended_by().size() != 1)
          || (n4.children().size() != 2));
}
