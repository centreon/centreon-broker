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

#ifndef CCB_NEB_INTERNAL_HH
#  define CCB_NEB_INTERNAL_HH

#  include <list>
#  include <map>
#  include <string>
#  include <utility>
#  include "com/centreon/broker/logging/backend.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/multiplexing/publisher.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/callback.hh"

CCB_BEGIN()
namespace neb {
  // Data elements.
  enum data_element {
    de_acknowledgement = 1,
    de_acknowledgement_removed,
    de_custom_variable,
    de_custom_variable_status,
    de_downtime,
    de_downtime_removed,
    de_event_handler,
    de_flapping_status,
    de_host_check,
    de_host_dependency,
    de_host_group,
    de_host_group_member,
    de_host,
    de_host_parent,
    de_host_status,
    de_instance,
    de_instance_status,
    de_log_entry,
    de_module,
    de_service_check,
    de_service_dependency,
    de_service_group,
    de_service_group_member,
    de_service,
    de_service_status
  };

  // Configuration file.
  extern QString gl_configuration_file;

  // List of host IDs.
  extern umap<std::string, int> gl_hosts;

  // List of service IDs.
  extern std::map<std::pair<std::string, std::string>, std::pair<int, int> >
    gl_services;

  // Sender object.
  extern multiplexing::publisher gl_publisher;

  // Registered callbacks.
  extern std::list<misc::shared_ptr<neb::callback> >
    gl_registered_callbacks;
}

CCB_END()

#endif // !CCB_NEB_INTERNAL_HH
