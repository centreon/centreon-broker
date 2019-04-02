/*
** Copyright 2014 Centreon
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
    std::shared_ptr<bam::ba>
      my_ba(new bam::ba);
    std::shared_ptr<bam::kpi_service>
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
      std::shared_ptr<neb::service_status>
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
