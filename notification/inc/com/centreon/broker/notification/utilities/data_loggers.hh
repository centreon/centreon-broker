/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH
#  define CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH

#  include <string>
#  include "com/centreon/broker/logging/logging.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace logging {
  // Constant ref needed because the temp_logger returned by logging
  // is a rvalue, and can only be acceded by a const reference.
  // Am I pushing the boundary of the language too far?
  temp_logger& operator<<(const temp_logger& left,
                          notification::node_id const& node) throw();
}

namespace notification {
  class data_logger {
  public:
    template <typename T>
    static void log_container(std::string const& container_name,
                              T& container) {
      logging::info(logging::low) << "Logging container called " <<
                                     container_name;
      for (typename T::iterator it(container.begin()), end(container.end());
           it != end; ++it)
      logging::info(logging::low) << (*it);
    }
  };
}

CCB_END()

#endif //!CCB_NOTIFICATION_UTILITIES_DATA_LOGGERS_TYPEDEF_HH
