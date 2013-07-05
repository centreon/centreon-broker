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
#  include "com/centreon/broker/neb/acknowledgement.hh"
#  include "com/centreon/broker/neb/callback.hh"

CCB_BEGIN()
namespace neb {
  // Configuration file.
  extern QString gl_configuration_file;

  // Instance information.
  extern unsigned int instance_id;
  extern QString      instance_name;

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

  // Acknowledgement list.
  extern std::map<std::pair<unsigned int, unsigned int>, neb::acknowledgement>
    gl_acknowledgements;
}

CCB_END()

#endif // !CCB_MODULE_INTERNAL_HH
