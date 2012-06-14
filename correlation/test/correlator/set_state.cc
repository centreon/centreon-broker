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

#include <QMap>
#include <QPair>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/correlator.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that state can be properly set to the correlator.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Create state. 
  QMap<QPair<unsigned int, unsigned int>, node> state;
  node& n1(state[qMakePair(42u, 24u)]);
  n1.host_id = 42;
  n1.service_id = 24;
  n1.state = 3;
  n1.my_issue.reset(new issue);
  n1.my_issue->host_id = 42;
  n1.my_issue->service_id = 24;
  n1.my_issue->start_time = 123456;
  node& n2(state[qMakePair(77u, 56u)]);
  n2.host_id = 77;
  n2.service_id = 56;
  n2.state = 2;
  n2.my_issue.reset(new issue);
  n2.my_issue->host_id = 77;
  n2.my_issue->service_id = 56;
  n2.my_issue->start_time = 7466;
  node& n3(state[qMakePair(123u, 0u)]);
  n3.host_id = 123;
  n3.service_id = 0;
  n3.state = 0;
  n1.add_parent(&n2);

  // Set state to correlator.
  correlator c;
  c.set_state(state);

  // Compare states.
  return (c.get_state() != state);
}
