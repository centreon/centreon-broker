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
#include "com/centreon/broker/correlation/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that state can be properly set to the correlator.
 *
 *  @return 0 on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  // Initialization.
  config::applier::init();

  try {
    // Create state.
    QMap<QPair<unsigned int, unsigned int>, node> state;
    node& n1(state[qMakePair(42u, 24u)]);
    n1.host_id = 42;
    n1.service_id = 24;
    n1.current_state = 3;
    n1.my_issue.reset(new issue);
    n1.my_issue->host_id = 42;
    n1.my_issue->service_id = 24;
    n1.my_issue->start_time = 123456;
    node& n2(state[qMakePair(77u, 56u)]);
    n2.host_id = 77;
    n2.service_id = 56;
    n2.current_state = 2;
    n2.my_issue.reset(new issue);
    n2.my_issue->host_id = 77;
    n2.my_issue->service_id = 56;
    n2.my_issue->start_time = 7466;
    node& n3(state[qMakePair(123u, 0u)]);
    n3.host_id = 123;
    n3.service_id = 0;
    n3.current_state = 0;
    n1.add_parent(&n2);

    // Set state to correlator.
    correlation::stream c("", std::shared_ptr<persistent_cache>(), false);
    c.set_state(state);

    // Compare states.
    if (c.get_state() != state)
      throw(exceptions::msg() << "state mismatch");

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
