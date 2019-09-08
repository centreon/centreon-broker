/*
** Copyright 2011-2015 Centreon
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

#include <QMap>
#include <QPair>
#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that parenting work.
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
    correlation::stream c("", std::shared_ptr<persistent_cache>(), false);
    {
      // Create state.
      QMap<QPair<unsigned int, unsigned int>, node> state;
      node& n1(state[qMakePair(42u, 24u)]);
      n1.host_id = 42;
      n1.service_id = 24;
      n1.current_state = 0;
      node& n2(state[qMakePair(56u, 13u)]);
      n2.host_id = 56;
      n2.service_id = 13u;
      n2.current_state = 0;
      node& n3(state[qMakePair(90u, 42u)]);
      n3.host_id = 90;
      n3.service_id = 42u;
      n3.current_state = 0;
      node& n4(state[qMakePair(213u, 8u)]);
      n4.host_id = 213;
      n4.service_id = 8u;
      n4.current_state = 0;
      n1.add_parent(&n2);
      n1.add_parent(&n3);
      n1.add_parent(&n4);

      // Apply state.
      c.set_state(state);
    }

    // Send node status.
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 56;
      ss->service_id = 13;
      ss->state_type = 1;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456789;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 90;
      ss->service_id = 42;
      ss->state_type = 1;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456790;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456791;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 213;
      ss->service_id = 8;
      ss->state_type = 1;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456792;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 56;
      ss->service_id = 13;
      ss->state_type = 1;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456793;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456794;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 90;
      ss->service_id = 42;
      ss->state_type = 1;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456795;
      c.write(ss);
    }
    {
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 213;
      ss->service_id = 8;
      ss->state_type = 1;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456796;
      c.write(ss);
    }

    // Check correlation content.
    multiplexing::engine::instance().stop();
    t.finalize();
    QList<std::shared_ptr<io::data> > content;
    add_issue(content, -1, -1, 56, 13, 123456789);
    add_issue(content, -1, -1, 90, 42, 123456790);
    add_issue(content, -1, -1, 42, 24, 123456791);
    add_issue(content, -1, -1, 213, 8, 123456792);
    add_issue_parent(content, 42, 24, 123456791, -1, 213, 8, 123456792,
                     123456792);
    add_issue_parent(content, 42, 24, 123456791, -1, 56, 13, 123456789,
                     123456792);
    add_issue_parent(content, 42, 24, 123456791, -1, 90, 42, 123456790,
                     123456792);
    add_issue_parent(content, 42, 24, 123456791, 123456793, 56, 13, 123456789,
                     123456792);
    add_issue_parent(content, 42, 24, 123456791, 123456793, 90, 42, 123456790,
                     123456792);
    add_issue_parent(content, 42, 24, 123456791, 123456793, 213, 8, 123456792,
                     123456792);
    add_issue(content, -1, 123456793, 56, 13, 123456789);
    add_issue(content, -1, 123456794, 42, 24, 123456791);
    add_issue(content, -1, 123456795, 90, 42, 123456790);
    add_issue(content, -1, 123456796, 213, 8, 123456792);

    // Check.
    check_content(t, content);

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
