/*
** Copyright 2009-2013,2015 Centreon
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

#ifndef CCB_NEB_INTERNAL_HH
#  define CCB_NEB_INTERNAL_HH

#  include <list>
#  include <map>
#  include <string>
#  include <utility>
#  include "com/centreon/broker/logging/backend.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/multiplexing/publisher.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/callback.hh"

CCB_BEGIN()

namespace neb {
  // Forward declaration.
  class   acknowledgement;

  // Data elements.
  enum data_element {
    de_acknowledgement = 1,
    de_comment,
    de_custom_variable,
    de_custom_variable_status,
    de_downtime,
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
    de_service_status,
    de_instance_configuration,
    de_responsive_instance
  };

  // Configuration file.
  extern std::string gl_configuration_file;

  // Sender object.
  extern multiplexing::publisher gl_publisher;

  // Registered callbacks.
  extern std::list<std::shared_ptr<neb::callback> >
    gl_registered_callbacks;

  // Acknowledgement list.
  extern std::map<std::pair<unsigned int, unsigned int>, neb::acknowledgement>
    gl_acknowledgements;
}

CCB_END()

#endif // !CCB_NEB_INTERNAL_HH
