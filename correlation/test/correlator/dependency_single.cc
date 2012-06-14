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
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that dependencies work.
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
  n1.state = 0;
  node& n2(state[qMakePair(56u, 13u)]);
  n2.host_id = 56;
  n2.service_id = 13u;
  n2.state = 0;
  n1.add_dependency(&n2);

  // Create correlator and apply state.
  correlator c;
  c.set_state(state);

  // Send node status.
  {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 2;
    c.write(ss.staticCast<io::data>());
  }
  {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 56;
    ss->service_id = 13;
    ss->state_type = 1;
    ss->current_state = 2;
    c.write(ss.staticCast<io::data>());
  }
  {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 56;
    ss->service_id = 13;
    ss->state_type = 1;
    ss->current_state = 0;
    c.write(ss.staticCast<io::data>());
  }
  {
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 0;
    c.write(ss.staticCast<io::data>());
  }

  // Check correlation content.
  QList<misc::shared_ptr<io::data> > content;
  add_issue(content, 0, 0, 42, 24, 1);
  add_issue(content, 0, 0, 56, 13, 1);
  add_issue_parent(content, 42, 24, 1, 0, 56, 13, 1, 1);
  add_issue_parent(content, 42, 24, 1, 1, 56, 13, 1, 1);
  add_issue(content, 0, 1, 56, 13, 1);
  add_issue(content, 0, 1, 42, 24, 1);
  return (!check_content(c, content));
}
