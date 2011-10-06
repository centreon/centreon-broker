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
  correlation::node cn(bn);

  // Reset base object.
  correlation::node dn(bn);
  dn.host_id = 23;
  dn.my_issue.reset();
  dn.service_id = 2347;
  dn.since = 553445;
  dn.state = 1;
  dn.remove_child(&n1);
  dn.remove_dependency(&n3);

  // Check equality.
  return (!(bn == cn)
          || (bn == dn)
          || (cn == dn)
          || !(bn == bn)
          || !(cn == cn)
          || !(dn == dn));
}
