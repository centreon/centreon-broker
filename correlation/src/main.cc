/*
** Copyright 2011-2016 Centreon
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

#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/factory.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/correlation/issue.hh"
#include "com/centreon/broker/correlation/issue_parent.hh"
#include "com/centreon/broker/correlation/log_issue.hh"
#include "com/centreon/broker/correlation/state.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;

// Load count.
static uint32_t instances(0);

extern "C" {
/**
 *  Module version symbol. Used to check for version mismatch.
 */
char const* broker_module_version = CENTREON_BROKER_VERSION;

/**
 *  Module deinitialization routine.
 */
void broker_module_deinit() {
  // Decrement instance number.
  if (!--instances) {
    // Unregister correlation layer.
    io::protocols::instance().unreg("correlation");

    // Remove events.
    io::events::instance().unregister_category(io::events::correlation);
  }
}

/**
 *  Module initialization routine.
 *
 *  @param[in] arg Configuration argument.
 */
void broker_module_init(void const* arg) {
  (void)arg;

  // Increment instance number.
  if (!instances++) {
    // Correlation module.
    logging::info(logging::high) << "correlation: module for Centreon Broker "
                                 << CENTREON_BROKER_VERSION;

    // Register correlation layer.
    io::protocols::instance().reg(
        "correlation", std::make_shared<correlation::factory>(), 1, 7);

    // Register category.
    io::events& e(io::events::instance());
    int correlation_category(
        e.register_category("correlation", io::events::correlation));
    if (correlation_category != io::events::correlation) {
      e.unregister_category(correlation_category);
      --instances;
      throw(exceptions::msg()
            << "correlation: category " << io::events::correlation
            << " is already registered whereas it should be "
            << "reserved for the correlation module");
    }

    // Register events.
    {
      e.register_event(
          io::events::correlation, correlation::de_engine_state,
          io::event_info("engine_state", &correlation::engine_state::operations,
                         correlation::engine_state::entries));
      e.register_event(
          io::events::correlation, correlation::de_state,
          io::event_info("state", &correlation::state::operations,
                         correlation::state::entries, "rt_servicestateevents"));
      e.register_event(
          io::events::correlation, correlation::de_issue,
          io::event_info("issue", &correlation::issue::operations,
                         correlation::issue::entries, "rt_issues", "issues"));
      e.register_event(
          io::events::correlation, correlation::de_issue_parent,
          io::event_info("issue_parent", &correlation::issue_parent::operations,
                         correlation::issue_parent::entries,
                         "rt_issues_issues_parents", "issues_issues_parents"));
      e.register_event(
          io::events::correlation, correlation::de_log_issue,
          io::event_info("log_issue", &correlation::log_issue::operations,
                         correlation::log_issue::entries, "log_logs", "logs"));
    }
  }
}
}
