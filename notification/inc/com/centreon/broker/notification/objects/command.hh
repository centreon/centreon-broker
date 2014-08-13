/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_COMMAND_HH
#  define CCB_NOTIFICATION_COMMAND_HH

#  include <string>

#  include "com/centreon/broker/neb/host_status.hh"
#  include "com/centreon/broker/neb/service_status.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace       notification {

  class         command {
  public:
    command(std::string const& base_command);
    command(command const& obj);
    command& operator=(command const& obj);

    std::string resolve(neb::host_status);
    std::string resolve(neb::service_status);

  private:
    command();

    std::string _base_command;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_COMMAND_HH
