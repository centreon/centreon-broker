/*
** Copyright 2011-2013 Merethis
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
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "com/centreon/broker/correlation/node.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/service_status.hh"
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
      ss->last_check = 123456789;
      c.write(ss.staticCast<io::data>());
    }
    { // #2
      misc::shared_ptr<neb::acknowledgement>
        ack(new neb::acknowledgement);
      ack->host_id = 42;
      ack->service_id = 24;
      ack->entry_time = 123456790;
      c.write(ack.staticCast<io::data>());
    }
    { // #3
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->current_state = 1;
      ss->last_check = 123456791;
      c.write(ss.staticCast<io::data>());
    }
    { // #4
      misc::shared_ptr<neb::acknowledgement>
        ack(new neb::acknowledgement);
      ack->host_id = 42;
      ack->service_id = 24;
      ack->entry_time = 123456792;
      c.write(ack.staticCast<io::data>());
    }
    { // #5
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->current_state = 0;
      ss->last_check = 123456793;
      c.write(ss.staticCast<io::data>());
    }
    { // #6
      misc::shared_ptr<neb::acknowledgement>
        ack(new neb::acknowledgement);
      ack->host_id = 42;
      ack->service_id = 24;
      ack->entry_time = 123456794;
      c.write(ack.staticCast<io::data>());
    }
    { // #7
      misc::shared_ptr<neb::service_status> ss(new neb::service_status);
      ss->host_id = 42;
      ss->service_id = 24;
      ss->state_type = 1;
      ss->current_state = 1;
      ss->last_check = 123456795;
      c.write(ss.staticCast<io::data>());
    }

    // Check correlation content.
    QList<misc::shared_ptr<io::data> > content;
    // #1
    add_state_service(content, -1, 0, 123456789, 42, false, 24, 0);
    add_state_service(content, -1, 2, 0, 42, false, 24, 123456789);
    add_issue(content, 0, 0, 42, 24, 123456789);
    // #2
    add_state_service(
      content,
      -1,
      2,
      123456790,
      42,
      false,
      24,
      123456789);
    add_state_service(
      content,
      123456790,
      2,
      0,
      42,
      false,
      24,
      123456790);
    add_issue(content, 123456790, 0, 42, 24, 123456789);
    // #3
    add_state_service(
      content,
      123456790,
      2,
      123456791,
      42,
      false,
      24,
      123456790);
    add_state_service(
      content,
      123456790,
      1,
      0,
      42,
      false,
      24,
      123456791);
    // #4 should not change anything.
    // #5
    add_state_service(
      content,
      123456790,
      1,
      123456793,
      42,
      false,
      24,
      123456791);
    add_state_service(content, -1, 0, 0, 42, false, 24, 123456793);
    add_issue(content, 123456790, 123456793, 42, 24, 123456789);
    // #6 should not change anything.
    // #7
    add_state_service(
      content,
      -1,
      0,
      123456795,
      42,
      false,
      24,
      123456793);
    add_state_service(content, -1, 1, 0, 42, false, 24, 123456795);
    add_issue(content, 0, 0, 42, 24, 123456795);

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

  return (retval);
}
