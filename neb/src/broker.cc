/*
** Copyright 2009-2013 Merethis
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
        io::events::instance().reg("neb", elements);
      }
    }

    return ;
  }
}
