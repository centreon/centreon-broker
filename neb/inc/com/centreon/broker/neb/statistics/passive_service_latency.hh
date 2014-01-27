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

#ifndef CCB_NEB_STATISTICS_PASSIVE_SERVICE_LATENCY_HH
#  define CCB_NEB_STATISTICS_PASSIVE_SERVICE_LATENCY_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/statistics/plugin.hh"

CCB_BEGIN()

namespace         neb {
  namespace       statistics {
    /**
     *  @class passive_service_latency passive_service_latency.hh "com/centreon/broker/neb/statistics/passive_service_latency.hh"
     *  @brief passive_service_latency statistics plugin.
     */
    class         passive_service_latency : public plugin {
    public:
                  passive_service_latency();
                  passive_service_latency(
                    passive_service_latency const& right);
                  ~passive_service_latency();
      passive_service_latency&
                  operator=(passive_service_latency const& right);
      void        run(std::string& output, std::string& perfdata);
    };
  }
}

CCB_END()

#endif // !CCB_NEB_STATISTICS_PASSIVE_SERVICE_LATENCY_HH
