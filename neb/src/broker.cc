/*
** Copyright 2009-2013,2015 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/node_events_factory.hh"
#include "com/centreon/broker/io/protocols.hh"

using namespace com::centreon::broker;

static unsigned int neb_instances(0);

extern "C" {
  /**
   *  Module deinitialization routine.
   */
  void broker_module_deinit() {
    if (!--neb_instances) {
      // Remove factory.
      io::protocols::instance().unreg("nodeevents");

      // Remove events.
      io::events::instance().unregister_category(io::events::neb);
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
    if (!neb_instances++) {
      logging::info(logging::high)
        << "NEB: module for Centreon Broker "
        << CENTREON_BROKER_VERSION;
      io::events& e(io::events::instance());

      // Register category.
      int neb_category(e.register_category("neb", io::events::neb));
      if (neb_category != io::events::neb) {
        e.unregister_category(neb_category);
        --neb_instances;
        throw (exceptions::msg() << "NEB: category " << io::events::neb
               << " is already registered whereas it should be "
               << "reserved for the NEB module");
      }

      // Register events.
      {
        e.register_event(
            io::events::neb,
            neb::de_acknowledgement,
            io::event_info(
                  "acknowledgement",
                  &neb::acknowledgement::operations,
                  neb::acknowledgement::entries));
        e.register_event(
            io::events::neb,
            neb::de_custom_variable,
            io::event_info(
                  "custom_variable",
                  &neb::custom_variable::operations,
                  neb::custom_variable::entries));
        e.register_event(
            io::events::neb,
            neb::de_custom_variable_status,
            io::event_info(
                  "custom_variable_status",
                  &neb::custom_variable_status::operations,
                  neb::custom_variable_status::entries));
        e.register_event(
            io::events::neb,
            neb::de_downtime,
            io::event_info(
                  "downtime",
                  &neb::downtime::operations,
                  neb::downtime::entries));
        e.register_event(
            io::events::neb,
            neb::de_event_handler,
            io::event_info(
                  "event_handler",
                  &neb::event_handler::operations,
                  neb::event_handler::entries));
        e.register_event(
            io::events::neb,
            neb::de_flapping_status,
            io::event_info(
                  "flapping_status",
                  &neb::flapping_status::operations,
                  neb::flapping_status::entries));
        e.register_event(
            io::events::neb,
            neb::de_host_check,
            io::event_info(
                  "host_check",
                  &neb::host_check::operations,
                  neb::host_check::entries));
        e.register_event(
            io::events::neb,
            neb::de_host_dependency,
            io::event_info(
                  "host_dependency",
                  &neb::host_dependency::operations,
                  neb::host_dependency::entries));
        e.register_event(
            io::events::neb,
            neb::de_host_group,
            io::event_info(
                  "host_group",
                  &neb::host_group::operations,
                  neb::host_group::entries));
        e.register_event(
            io::events::neb,
            neb::de_host_group_member,
            io::event_info(
                  "host_group_member",
                  &neb::host_group_member::operations,
                  neb::host_group_member::entries));
        e.register_event(
            io::events::neb,
            neb::de_host,
            io::event_info(
                  "host",
                  &neb::host::operations,
                  neb::host::entries));
        e.register_event(
            io::events::neb,
            neb::de_host_parent,
            io::event_info(
                  "host_parent",
                  &neb::host_parent::operations,
                  neb::host_parent::entries));
        e.register_event(
            io::events::neb,
            neb::de_host_status,
            io::event_info(
                  "host_status",
                  &neb::host_status::operations,
                  neb::host_status::entries));
        e.register_event(
            io::events::neb,
            neb::de_instance,
            io::event_info(
                  "instance",
                  &neb::instance::operations,
                  neb::instance::entries));
        e.register_event(
            io::events::neb,
            neb::de_instance_status,
            io::event_info(
                  "instance_status",
                  &neb::instance_status::operations,
                  neb::instance_status::entries));
        e.register_event(
            io::events::neb,
            neb::de_log_entry,
            io::event_info(
                  "log_entry",
                  &neb::log_entry::operations,
                  neb::log_entry::entries));
        e.register_event(
            io::events::neb,
            neb::de_module,
            io::event_info(
                  "module",
                  &neb::module::operations,
                  neb::module::entries));
        e.register_event(
            io::events::neb,
            neb::de_service_check,
            io::event_info(
                  "service_check",
                  &neb::service_check::operations,
                  neb::service_check::entries));
        e.register_event(
            io::events::neb,
            neb::de_service_dependency,
            io::event_info(
                  "service_dependency",
                  &neb::service_dependency::operations,
                  neb::service_dependency::entries));
        e.register_event(
            io::events::neb,
            neb::de_service_group,
            io::event_info(
                  "service_group",
                  &neb::service_group::operations,
                  neb::service_group::entries));
        e.register_event(
            io::events::neb,
            neb::de_service_group_member,
            io::event_info(
                  "service_group_member",
                  &neb::service_group_member::operations,
                  neb::service_group_member::entries));
        e.register_event(
            io::events::neb,
            neb::de_service,
            io::event_info(
                  "service",
                  &neb::service::operations,
                  neb::service::entries));
        e.register_event(
            io::events::neb,
            neb::de_service_status,
            io::event_info(
                  "service_status",
                  &neb::service_status::operations,
                  neb::service_status::entries));
      }

      // Register neb layer.
      io::protocols::instance().reg(
                                  "nodeevents",
                                  neb::node_events_factory(),
                                  1,
                                  7);
    }

    return ;
  }
}
