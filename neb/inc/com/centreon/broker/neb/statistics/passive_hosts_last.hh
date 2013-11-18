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

#ifndef CCB_NEB_STATISTICS_PASSIVE_SERVICES_LAST_HH
#  define CCB_NEB_STATISTICS_PASSIVE_SERVICES_LAST_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/statistics/plugin.hh"

CCB_BEGIN()

namespace         neb {
  namespace       statistics {
    /**
     *  @class passive_hosts_last passive_hosts_last.hh "com/centreon/broker/neb/statistics/passive_hosts_last.hh"
     *  @brief passive_hosts_last statistics plugin.
     */
    class         passive_hosts_last : public plugin {
    public:
                  passive_hosts_last();
                  passive_hosts_last(
                    passive_hosts_last const& right);
                  ~passive_hosts_last();
      passive_hosts_last&
                  operator=(passive_hosts_last const& right);
      std::string run();
    };
  }
}

CCB_END()

#endif // !CCB_NEB_STATISTICS_PASSIVE_SERVICES_LAST_HH
