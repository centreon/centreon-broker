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

#include "com/centreon/broker/neb/acknowledgement.hh"
#include <QMap>
#include <QPair>
#include <cstdlib>
#include <iostream>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

// CAUTION
// This is evil code. Do not modify unless you wanna spend some hours
// debugging for a forgotten magic value hidden somewhere.

/**
 *  Check that node is properly default constructed.
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
    // Create state.
    QMap<QPair<unsigned int, unsigned int>, node> state;
    node& n(state[qMakePair(42u, 24u)]);
    n.host_id = 42;
    n.service_id = 24;
    n.current_state = 0;
    n.start_time = 0;

    // Create correlator and apply state.
    correlation::stream c("", std::shared_ptr<persistent_cache>(), false);
    c.set_state(state);

    // Send node status.
    {  // #1
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456789;
      c.write(ss);
    }
    {  // #2
      std::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);
      ack->host_id = 42;
      ack->service_id = 24;
      ack->entry_time = 123456790;
      ack->is_sticky = false;
      c.write(ack);
    }
    {  // #3
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 1;
      ss->last_hard_state_change = 123456791;
      c.write(ss);
    }
    {  // #4
      std::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);
      ack->host_id = 42;
      ack->service_id = 24;
      ack->entry_time = 123456792;
      ack->is_sticky = true;
      c.write(ack);
    }
    {  // #5
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 2;
      ss->last_hard_state_change = 123456793;
      c.write(ss);
    }
    {  // #6
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 0;
      ss->last_hard_state_change = 123456794;
      c.write(ss);
    }
    {  // #7
      std::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);
      ack->host_id = 42;
      ack->service_id = 24;
      ack->entry_time = 123456795;
      c.write(ack);
    }
    {  // #8
      std::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->last_hard_state = 1;
      ss->last_hard_state_change = 123456796;
      c.write(ss);
    }

    // Check correlation content.
    multiplexing::engine::instance().stop();
    t.finalize();
    QList<std::shared_ptr<io::data> > content;
    // #1
    add_state(content, -1, 0, 123456789, 42, false, 24, 0);
    add_state(content, -1, 2, -1, 42, false, 24, 123456789);
    add_issue(content, -1, -1, 42, 24, 123456789);
    // #2
    add_state(content, -1, 2, 123456790, 42, false, 24, 123456789);
    add_state(content, 123456790, 2, -1, 42, false, 24, 123456790);
    // #3
    add_state(content, 123456790, 2, 123456791, 42, false, 24, 123456790);
    add_state(content, -1, 1, -1, 42, false, 24, 123456791);
    // #4
    add_state(content, -1, 1, 123456792, 42, false, 24, 123456791);
    add_state(content, 123456792, 1, -1, 42, false, 24, 123456792);
    // #5
    add_state(content, 123456792, 1, 123456793, 42, false, 24, 123456792);
    add_state(content, 123456793, 2, -1, 42, false, 24, 123456793);
    // #6
    add_state(content, 123456793, 2, 123456794, 42, false, 24, 123456793);
    add_state(content, -1, 0, -1, 42, false, 24, 123456794);
    add_issue(content, 123456790, 123456794, 42, 24, 123456789);
    // #7, should not change anything
    // #8
    add_state(content, -1, 0, 123456796, 42, false, 24, 123456794);
    add_state(content, -1, 1, -1, 42, false, 24, 123456796);
    add_issue(content, -1, -1, 42, 24, 123456796);

    // Check.
    check_content(t, content);

    // Success.
    retval = EXIT_SUCCESS;
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  return (retval);
}
