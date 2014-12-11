/*
** Copyright 2014 Merethis
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
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/kpi_service.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;

#define RECOMPUTE_LIMIT 100

/**
 *  Check that a KPI change at BA recompute does not mess with the BA
 *  value.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  bool error(true);
  try {
    // Build BAM objects.
    misc::shared_ptr<bam::ba>
      my_ba(new bam::ba);
    misc::shared_ptr<bam::kpi_service>
      my_kpi(new bam::kpi_service);
    my_kpi->set_host_id(1);
    my_kpi->set_service_id(1);
    my_kpi->set_impact_critical(100.0);
    my_kpi->set_state_hard(0);
    my_kpi->set_state_soft(my_kpi->get_state_hard());
    my_ba->add_impact(my_kpi);
    my_kpi->add_parent(my_ba);

    // Change KPI state as much time as needed to trigger a
    // recomputation. Note that the loop must terminate on a odd number
    // for the test to be correct.
    time_t now(time(NULL));
    for (int i(0); i < RECOMPUTE_LIMIT + 2; ++i) {
      misc::shared_ptr<neb::service_status>
        ss(new neb::service_status);
      ss->host_id = 1;
      ss->service_id = 1;
      ss->last_check = now + i;
      // Last value of i needs to send a OK (0) state to the KPI.
      ss->last_hard_state = ((i & 1) ? 0 : 2);
      ss->current_state = ss->last_hard_state;
      my_kpi->service_update(ss);
    }

    // Success.
    short kpi_state(my_kpi->get_state_hard());
    short ba_state(my_ba->get_state_hard());
    std::cout << "KPI state is " << kpi_state << "\n"
              << "BA state is " << ba_state << "\n";
    error = (ba_state != 0);
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception";
  }

  // Cleanup.
  config::applier::deinit();

  // Return value.
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
