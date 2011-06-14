/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_MODULE_INTERNAL_HH_
# define CCB_MODULE_INTERNAL_HH_

# include <map>
# include <string>
# include <utility>
# include "logging/backend.hh"
# include "multiplexing/publisher.hh"

namespace       com {
  namespace     centreon {
    namespace   broker {
      namespace module {

        // Configuration file.
        extern QString gl_configuration_file;

        // Instance information.
        extern unsigned int instance_id;
        extern QString      instance_name;

        // List of host IDs.
        extern std::map<std::string, int> gl_hosts;

        // List of service IDs.
        extern std::map<std::pair<std::string, std::string>, std::pair<int, int> > gl_services;

        // Sender object.
        extern com::centreon::broker::multiplexing::publisher gl_publisher;
      }
    }
  }
}

#endif /* !CCB_MODULE_INTERNAL_HH_ */
