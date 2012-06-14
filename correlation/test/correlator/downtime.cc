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
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/neb/service_status.hh"
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
  node& n(state[qMakePair(42u, 24u)]);
  n.host_id = 42;
  n.service_id = 24;
  n.state = 0;

  // Create correlator and apply state.
  correlator c;
  c.set_state(state);

  // Send node status.
  { // #1
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 2;
    c.write(ss.staticCast<io::data>());
  }
  { // #2
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 2;
    ss->scheduled_downtime_depth = 1;
    c.write(ss.staticCast<io::data>());
  }
  { // #3
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 0;
    c.write(ss.staticCast<io::data>());
  }
  { // #4
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 0;
    ss->scheduled_downtime_depth = 1;
    c.write(ss.staticCast<io::data>());
  }
  { // #5
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 2;
    ss->scheduled_downtime_depth = 1;
    c.write(ss.staticCast<io::data>());
  }
  { // #6
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 0;
    ss->scheduled_downtime_depth = 1;
    c.write(ss.staticCast<io::data>());
  }
  { // #7
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    ss->host_id = 42;
    ss->service_id = 24;
    ss->state_type = 1;
    ss->current_state = 0;
    ss->scheduled_downtime_depth = 0;
    c.write(ss.staticCast<io::data>());
  }

  // Check correlation content.
  QList<misc::shared_ptr<io::data> > content;
  // #1
  add_state_service(content, 0, 0, 1, 42, false, 24, 0);
  add_state_service(content, 0, 2, 0, 42, false, 24, 1);
  add_issue(content, 0, 0, 42, 24, 1);
  // #2
  add_state_service(content, 0, 2, 1, 42, false, 24, 1);
  add_state_service(content, 0, 2, 0, 42, true, 24, 1);
  // #3
  add_state_service(content, 0, 2, 1, 42, true, 24, 1);
  add_state_service(content, 0, 0, 0, 42, false, 24, 1);
  add_issue(content, 0, 1, 42, 24, 1);
  // #4
  add_state_service(content, 0, 0, 1, 42, false, 24, 1);
  add_state_service(content, 0, 0, 0, 42, true, 24, 1);
  // #5
  add_state_service(content, 0, 0, 1, 42, true, 24, 1);
  add_state_service(content, 0, 2, 0, 42, true, 24, 1);
  add_issue(content, 0, 0, 42, 24, 1);
  // #6
  add_state_service(content, 0, 2, 1, 42, true, 24, 1);
  add_state_service(content, 0, 0, 0, 42, true, 24, 1);
  add_issue(content, 0, 1, 42, 24, 1);
  // #7
  add_state_service(content, 0, 0, 1, 42, true, 24, 1);
  add_state_service(content, 0, 0, 0, 42, false, 24, 1);
  return (!check_content(c, content));
}
