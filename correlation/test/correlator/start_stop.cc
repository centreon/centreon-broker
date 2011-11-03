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

#include <QList>
#include <QMap>
#include <QPair>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that node is properly default constructed.
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
  n1.add_parent(&n2);

  // Create correlator.
  correlator c;
  c.set_state(state);

  // Start correlator then stop it.
  c.starting();
  c.stopping();

  // Check correlation content.
  QList<QSharedPointer<io::data> > content;
  add_engine_state(content, true);
  /*add_issue_parent(content, 42, 24, 1, 1, 77, 56, 1, 1);
  add_issue(content, 0, 1, 42, 24, 1);
  add_issue(content, 0, 1, 77, 56, 1);
  add_engine_state(content, false);*/
  return (!check_content(c, content));
}
