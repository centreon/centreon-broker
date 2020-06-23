/*
** Copyright 2009-2013,2015-2016 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/protocols.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

static uint32_t neb_instances(0);

extern "C" {
/**
 *  Module version symbol. Used to check for version mismatch.
 */
char const* broker_module_version = CENTREON_BROKER_VERSION;

/**
 *  Module deinitialization routine.
 */
void broker_module_deinit() {
  if (!--neb_instances) {
    //      // Remove factory.
    //      io::protocols::instance().unreg("node_events");

    // Remove events.
    io::events::instance().unregister_category(io::events::neb);
  }
}

/**
 *  Module initialization routine.
 *
 *  @param[in] arg Configuration object.
 */
void broker_module_init(void const* arg) {
  (void)arg;
  if (!neb_instances++) {
    logging::info(logging::high)
        << "NEB: module for Centreon Broker " << CENTREON_BROKER_VERSION;
    io::events& e(io::events::instance());

    // Register category.
    int neb_category(e.register_category("neb", io::events::neb));
    if (neb_category != io::events::neb) {
      e.unregister_category(neb_category);
      --neb_instances;
      throw(exceptions::msg() << "NEB: category " << io::events::neb
                              << " is already registered whereas it should be "
                              << "reserved for the NEB module");
    }

    // Register events.
    {
      e.register_event(io::events::neb, neb::de_acknowledgement,
                       "acknowledgement", &neb::acknowledgement::operations,
                       neb::acknowledgement::entries, "acknowledgements");
      e.register_event(io::events::neb, neb::de_comment, "comment",
                       &neb::comment::operations, neb::comment::entries,
                       "comments");
      e.register_event(io::events::neb, neb::de_custom_variable,
                       "custom_variable", &neb::custom_variable::operations,
                       neb::custom_variable::entries, "customvariables");
      e.register_event(io::events::neb, neb::de_custom_variable_status,
                       "custom_variable_status",
                       &neb::custom_variable_status::operations,
                       neb::custom_variable_status::entries, "customvariables");
      e.register_event(io::events::neb, neb::de_downtime, "downtime",
                       &neb::downtime::operations, neb::downtime::entries,
                       "downtimes");
      e.register_event(io::events::neb, neb::de_event_handler, "event_handler",
                       &neb::event_handler::operations,
                       neb::event_handler::entries, "eventhandlers");
      e.register_event(io::events::neb, neb::de_flapping_status,
                       "flapping_status", &neb::flapping_status::operations,
                       neb::flapping_status::entries, "flappingstatuses");
      e.register_event(io::events::neb, neb::de_host_check, "host_check",
                       &neb::host_check::operations, neb::host_check::entries,
                       "hosts");
      e.register_event(
          io::events::neb, neb::de_host_dependency, "host_dependency",
          &neb::host_dependency::operations, neb::host_dependency::entries,
          "hosts_hosts_dependencies");
      e.register_event(io::events::neb, neb::de_host, "host",
                       &neb::host::operations, neb::host::entries, "hosts");
      e.register_event(io::events::neb, neb::de_host_group, "host_group",
                       &neb::host_group::operations, neb::host_group::entries,
                       "hostgroups");
      e.register_event(io::events::neb, neb::de_host_group_member,
                       "host_group_member", &neb::host_group_member::operations,
                       neb::host_group_member::entries, "hosts_hostgroups");
      e.register_event(io::events::neb, neb::de_host_parent, "host_parent",
                       &neb::host_parent::operations, neb::host_parent::entries,
                       "hosts_hosts_parents");
      e.register_event(io::events::neb, neb::de_host_status, "host_status",
                       &neb::host_status::operations, neb::host_status::entries,
                       "hosts");
      e.register_event(io::events::neb, neb::de_instance, "instance",
                       &neb::instance::operations, neb::instance::entries,
                       "instances");
      e.register_event(io::events::neb, neb::de_instance_status,
                       "instance_status", &neb::instance_status::operations,
                       neb::instance_status::entries,
                       "instances");
      e.register_event(io::events::neb, neb::de_log_entry, "log_entry",
                       &neb::log_entry::operations, neb::log_entry::entries,
                       "logs");
      e.register_event(io::events::neb, neb::de_module, "module",
                       &neb::module::operations, neb::module::entries,
                       "modules");
      e.register_event(io::events::neb, neb::de_service_check, "service_check",
                       &neb::service_check::operations,
                       neb::service_check::entries, "services");
      e.register_event(
          io::events::neb, neb::de_service_dependency, "service_dependency",
          &neb::service_dependency::operations,
          neb::service_dependency::entries, "services_services_dependencies");
      e.register_event(io::events::neb, neb::de_service, "service",
                       &neb::service::operations, neb::service::entries,
                       "services");
      e.register_event(io::events::neb, neb::de_service_group, "service_group",
                       &neb::service_group::operations,
                       neb::service_group::entries, "servicegroups");
      e.register_event(
          io::events::neb, neb::de_service_group_member, "service_group_member",
          &neb::service_group_member::operations,
          neb::service_group_member::entries,"services_servicegroups");
      e.register_event(io::events::neb, neb::de_service_status,
                       "service_status", &neb::service_status::operations,
                       neb::service_status::entries, "services");
      e.register_event(io::events::neb, neb::de_instance_configuration,
                       "instance_configuration",
                       &neb::instance_configuration::operations,
                       neb::instance_configuration::entries);
      e.register_event(io::events::neb, neb::de_responsive_instance,
                       "responsive_instance",
                       &neb::responsive_instance::operations,
                       neb::responsive_instance::entries);
    }

    //      // Register neb layer.
    //      io::protocols::instance().reg(
    //                                  "node_events",
    //                                  neb::node_events_factory(),
    //                                  1,
    //                                  7);
  }
}
}
