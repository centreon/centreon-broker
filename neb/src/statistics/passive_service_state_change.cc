/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/neb/statistics/passive_service_state_change.hh"
#include <iomanip>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/statistics/compute_value.hh"
#include "com/centreon/engine/globals.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::neb::statistics;
using namespace com::centreon::engine;

/**
 *  Default constructor.
 */
passive_service_state_change::passive_service_state_change()
    : plugin("passive_service_state_change") {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
passive_service_state_change::passive_service_state_change(
    passive_service_state_change const& right)
    : plugin(right) {}

/**
 *  Destructor.
 */
passive_service_state_change::~passive_service_state_change() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
passive_service_state_change& passive_service_state_change::operator=(
    passive_service_state_change const& right) {
  plugin::operator=(right);
  return (*this);
}

/**
 *  Get statistics.
 *
 *  @param[out] output   The output return by the plugin.
 *  @param[out] perfdata The perf data return by the plugin.
 */
void passive_service_state_change::run(std::string& output,
                                       std::string& perfdata) {
  compute_value<double> cv;
  for (service_map::const_iterator it{service::services.begin()},
       end{service::services.end()};
       it != end; ++it)
    if (it->second->get_check_type() == checkable::check_passive)
      cv << it->second->get_percent_state_change();

  if (cv.size()) {
    // Output.
    std::ostringstream oss;
    oss << "Engine " << config::applier::state::instance().poller_name()
        << " has an average passive service state change of " << std::fixed
        << std::setprecision(2) << cv.avg() << "%";
    output = oss.str();

    // Perfdata.
    oss.str("");
    oss << "avg=" << cv.avg() << "% min=" << cv.min() << "% max=" << cv.max()
        << "%";
    perfdata = oss.str();
  } else {
    // Output.
    output =
        "No passive service to compute passive service "
        "state change on " +
        config::applier::state::instance().poller_name();
  }
  return;
}
