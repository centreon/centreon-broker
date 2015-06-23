/*
** Copyright 2011-2015 Merethis
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

#include <cstdlib>
#include <iostream>
#include <QMap>
#include <QPair>
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that dependencies work.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Initialization.
  config::applier::init();
  multiplexing::engine::load();
  // Start the multiplexing engine.
  test_stream t;
  multiplexing::engine::instance().hook(t);
  multiplexing::engine::instance().start();

  try {
    correlation::stream c("", misc::shared_ptr<persistent_cache>(), false);
    {
      // Create state.
      QMap<QPair<unsigned int, unsigned int>, node> state;
      node& n1(state[qMakePair(42u, 24u)]);
      n1.host_id = 42;
      n1.service_id = 24;
      n1.state = 0;
      node& n2(state[qMakePair(56u, 13u)]);
      n2.host_id = 56;
      n2.service_id = 13;
      n2.state = 0;
      node& n3(state[qMakePair(90u, 0u)]);
      n3.host_id = 90;
      n3.service_id = 0;
      n3.state = 0;
      n2.add_dependency(&n1);
      n3.add_dependency(&n1);

      // Create correlator and apply state.
      c.set_state(state);
    }

    // Send node status.
    { // #1
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 56;
      ss->service_id = 13;
      ss->state_type = 1;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456789;
      c.write(ss);
    }
    { // #2
      misc::shared_ptr<neb::host_status> hs(new neb::host_status);
      hs->host_id = 90;
      hs->state_type = 1;
      hs->last_hard_state = 1;
      hs->last_hard_state_change = 123456790;
      c.write(hs);
    }
    { // #3
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456791;
      c.write(ss);
    }
    { // #4
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 56;
      ss->service_id = 13;
      ss->state_type = 1;
      ss->last_hard_state = 1;
      ss->last_hard_state_change = 123456792;
      c.write(ss);
    }
    { // #5
      misc::shared_ptr<neb::host_status> hs(new neb::host_status);
      hs->host_id = 90;
      hs->state_type = 1;
      hs->last_hard_state = 2;
      hs->last_hard_state_change = 123456793;
      c.write(hs);
    }
    { // #6
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456794;
      c.write(ss);
    }

    // Check correlation content.
    multiplexing::engine::instance().stop();
    t.finalize();
    QList<misc::shared_ptr<io::data> > content;
    // #1
    add_state(content, -1, 0, 123456789, 56, false, 13, 0);
    add_state(content, -1, 2, -1, 56, false, 13, 123456789);
    add_issue(content, -1, -1, 56, 13, 123456789);
    // #2
    add_state(content, -1, 0, 123456790, 90, false, 0, 0);
    add_state(content, -1, 1, -1, 90, false, 0, 123456790);
    add_issue(content, -1, -1, 90, 0, 123456790);
    // #3
    add_state(content, -1, 0, 123456791, 42, false, 24, 0);
    add_state(content, -1, 2, -1, 42, false, 24, 123456791);
    add_issue(content, -1, -1, 42, 24, 123456791);
    add_issue_parent(
      content,
      56,
      13,
      123456789,
      -1,
      42,
      24,
      123456791,
      123456791);
    add_issue_parent(
      content,
      90,
      0,
      123456790,
      -1,
      42,
      24,
      123456791,
      123456791);
    // #4
    add_state(
      content,
      -1,
      2,
      123456792,
      56,
      false,
      13,
      123456789);
    add_state(content, -1, 1, -1, 56, false, 13, 123456792);
    // #5
    add_state(content, -1, 1, 123456793, 90, false, 0, 123456790);
    add_state(content, -1, 2, -1, 90, false, 0, 123456793);
    // #6
    add_state(
      content,
      -1,
      2,
      123456794,
      42,
      false,
      24,
      123456791);
    add_state(content, -1, 0, -1, 42, false, 24, 123456794);
    add_issue_parent(
      content,
      56,
      13,
      123456789,
      123456794,
      42,
      24,
      123456791,
      123456791);
    add_issue_parent(
      content,
      90,
      0,
      123456790,
      123456794,
      42,
      24,
      123456791,
      123456791);
    add_issue(content, -1, 123456794, 42, 24, 123456791);

    // Check.
    check_content(t, content);

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
