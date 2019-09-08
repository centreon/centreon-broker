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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"
//#include "com/centreon/broker/neb/node_events_factory.hh"
#include "com/centreon/broker/io/protocols.hh"

using namespace com::centreon::broker;

static unsigned int neb_instances(0);

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
      e.register_event(
          io::events::neb, neb::de_acknowledgement,
          io::event_info("acknowledgement", &neb::acknowledgement::operations,
                         neb::acknowledgement::entries, "rt_acknowledgements",
                         "acknowledgements"));
      e.register_event(io::events::neb, neb::de_comment,
                       io::event_info("comment", &neb::comment::operations,
                                      neb::comment::entries, "", "comments"));
      e.register_event(
          io::events::neb, neb::de_custom_variable,
          io::event_info("custom_variable", &neb::custom_variable::operations,
                         neb::custom_variable::entries, "rt_customvariables",
                         "customvariables"));
      e.register_event(io::events::neb, neb::de_custom_variable_status,
                       io::event_info("custom_variable_status",
                                      &neb::custom_variable_status::operations,
                                      neb::custom_variable_status::entries,
                                      "rt_customvariables", "customvariables"));
      e.register_event(
          io::events::neb, neb::de_downtime,
          io::event_info("downtime", &neb::downtime::operations,
                         neb::downtime::entries, "rt_downtimes", "downtimes"));
      e.register_event(
          io::events::neb, neb::de_event_handler,
          io::event_info("event_handler", &neb::event_handler::operations,
                         neb::event_handler::entries, "rt_eventhandlers",
                         "eventhandlers"));
      e.register_event(
          io::events::neb, neb::de_flapping_status,
          io::event_info("flapping_status", &neb::flapping_status::operations,
                         neb::flapping_status::entries, "rt_flappingstatuses",
                         "flappingstatuses"));
      e.register_event(
          io::events::neb, neb::de_host_check,
          io::event_info("host_check", &neb::host_check::operations,
                         neb::host_check::entries, "rt_hosts", "hosts"));
      e.register_event(
          io::events::neb, neb::de_host_dependency,
          io::event_info("host_dependency", &neb::host_dependency::operations,
                         neb::host_dependency::entries,
                         "rt_hosts_hosts_dependencies",
                         "hosts_hosts_dependencies"));
      e.register_event(io::events::neb, neb::de_host,
                       io::event_info("host", &neb::host::operations,
                                      neb::host::entries, "rt_hosts", "hosts"));
      e.register_event(
          io::events::neb, neb::de_host_group,
          io::event_info("host_group", &neb::host_group::operations,
                         neb::host_group::entries, "", "hostgroups"));
      e.register_event(io::events::neb, neb::de_host_group_member,
                       io::event_info("host_group_member",
                                      &neb::host_group_member::operations,
                                      neb::host_group_member::entries, "",
                                      "hosts_hostgroups"));
      e.register_event(
          io::events::neb, neb::de_host_parent,
          io::event_info("host_parent", &neb::host_parent::operations,
                         neb::host_parent::entries, "rt_hosts_hosts_parents",
                         "hosts_hosts_parents"));
      e.register_event(
          io::events::neb, neb::de_host_status,
          io::event_info("host_status", &neb::host_status::operations,
                         neb::host_status::entries, "rt_hosts", "hosts"));
      e.register_event(
          io::events::neb, neb::de_instance,
          io::event_info("instance", &neb::instance::operations,
                         neb::instance::entries, "rt_instances", "instances"));
      e.register_event(
          io::events::neb, neb::de_instance_status,
          io::event_info("instance_status", &neb::instance_status::operations,
                         neb::instance_status::entries, "rt_instances",
                         "instances"));
      e.register_event(
          io::events::neb, neb::de_log_entry,
          io::event_info("log_entry", &neb::log_entry::operations,
                         neb::log_entry::entries, "log_logs", "logs"));
      e.register_event(
          io::events::neb, neb::de_module,
          io::event_info("module", &neb::module::operations,
                         neb::module::entries, "rt_modules", "modules"));
      e.register_event(
          io::events::neb, neb::de_service_check,
          io::event_info("service_check", &neb::service_check::operations,
                         neb::service_check::entries, "rt_services",
                         "services"));
      e.register_event(io::events::neb, neb::de_service_dependency,
                       io::event_info("service_dependency",
                                      &neb::service_dependency::operations,
                                      neb::service_dependency::entries,
                                      "rt_services_services_dependencies",
                                      "services_services_dependencies"));
      e.register_event(
          io::events::neb, neb::de_service,
          io::event_info("service", &neb::service::operations,
                         neb::service::entries, "rt_services", "services"));
      e.register_event(
          io::events::neb, neb::de_service_group,
          io::event_info("service_group", &neb::service_group::operations,
                         neb::service_group::entries, "", "servicegroups"));
      e.register_event(io::events::neb, neb::de_service_group_member,
                       io::event_info("service_group_member",
                                      &neb::service_group_member::operations,
                                      neb::service_group_member::entries, "",
                                      "services_servicegroups"));
      e.register_event(
          io::events::neb, neb::de_service_status,
          io::event_info("service_status", &neb::service_status::operations,
                         neb::service_status::entries, "rt_services",
                         "services"));
      e.register_event(io::events::neb, neb::de_instance_configuration,
                       io::event_info("instance_configuration",
                                      &neb::instance_configuration::operations,
                                      neb::instance_configuration::entries));
      e.register_event(io::events::neb, neb::de_responsive_instance,
                       io::event_info("responsive_instance",
                                      &neb::responsive_instance::operations,
                                      neb::responsive_instance::entries));
    }

    //      // Register neb layer.
    //      io::protocols::instance().reg(
    //                                  "node_events",
    //                                  neb::node_events_factory(),
    //                                  1,
    //                                  7);
  }

  return;
}
}
