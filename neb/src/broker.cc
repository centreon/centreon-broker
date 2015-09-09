/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

static unsigned int instances(0);

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    if (!--instances) {
      // Remove events.
      io::events::instance().unreg("neb");
    }
    return ;
  }

  /**
   *  Module initialization routine.
   *
   *  @param[in] arg Configuration object.
   */
  void broker_module_init(void const* arg) {
    (void)arg;
    if (!instances++) {
      logging::info(logging::high)
        << "NEB: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;

      // Register events.
      {
        std::set<unsigned int> elements;
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_acknowledgement>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_comment>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_custom_variable>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_custom_variable_status>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_downtime>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_event_handler>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_flapping_status>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host_check>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host_dependency>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host_group>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host_group_member>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host_parent>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_host_status>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_instance>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_instance_status>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_log_entry>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_module>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_notification>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_service_check>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_service_dependency>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_service_group>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_service_group_member>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_service>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_service_status>::value);
        elements.insert(
                   io::events::data_type<io::events::neb, neb::de_instance_configuration>::value);
        io::events::instance().reg("neb", elements);
      }
    }

    return ;
  }
}
