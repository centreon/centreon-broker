/*
** Copyright 2013 Merethis
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

#ifndef CCB_NEB_STATISTICS_HOSTS_PASSIVELY_CHECKED_HH
#  define CCB_NEB_STATISTICS_HOSTS_PASSIVELY_CHECKED_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/statistics/plugin.hh"

CCB_BEGIN()

namespace                neb {
  namespace              statistics {
    /**
     *  @class hosts_passively_checked hosts_passively_checked.hh "com/centreon/broker/neb/statistics/hosts_passively_checked.hh"
     *  @brief hosts_passively_checked statistics plugin.
     */
    class                hosts_passively_checked : public plugin {
    public:
                         hosts_passively_checked();
                         hosts_passively_checked(
                           hosts_passively_checked const& right);
                         ~hosts_passively_checked();
      hosts_passively_checked&
                         operator=(hosts_passively_checked const& right);
      void               run(std::string& output, std::string& perfdata);
    };
  }
}

CCB_END()

#endif // !CCB_NEB_STATISTICS_HOSTS_PASSIVELY_CHECKED_HH
