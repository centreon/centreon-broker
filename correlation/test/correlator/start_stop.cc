/*
** Copyright 2011-2014 Centreon
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

#include <cstdlib>
#include <iostream>
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
 *  @return EXIT_SUCCESS on success.
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
    n1.state = 3;
    n1.my_issue.reset(new issue);
    n1.my_issue->host_id = 42;
    n1.my_issue->service_id = 24;
    n1.my_issue->start_time = 123456789;
    node& n2(state[qMakePair(77u, 56u)]);
    n2.host_id = 77;
    n2.service_id = 56;
    n2.state = 2;
    n2.my_issue.reset(new issue);
    n2.my_issue->host_id = 77;
    n2.my_issue->service_id = 56;
    n2.my_issue->start_time = 123456790;
    n1.add_parent(&n2);

    // Create correlator.
    correlator c(0);
    c.set_state(state);

    // Start correlator then stop it.
    c.starting();
    c.stopping();

    // Check correlation content.
    QList<misc::shared_ptr<io::data> > content;
    add_engine_state(content, true);
    // add_issue_parent(content, 42, 24, 1, 1, 77, 56, 1, 1);
    // add_issue(content, 0, 1, 42, 24, 1);
    // add_issue(content, 0, 1, 77, 56, 1);
    add_engine_state(content, false);

    // Check.
    check_content(c, content);

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
