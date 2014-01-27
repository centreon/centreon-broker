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

#ifndef CCB_NEB_STATISTICS_GENERATOR_HH
#  define CCB_NEB_STATISTICS_GENERATOR_HH

#  include <map>
#  include <string>
#  include <utility>
#  include "com/centreon/broker/config/state.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/statistics/plugin.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

CCB_BEGIN()

namespace          neb {
  namespace        statistics {
    /**
     *  @class generator generator.hh "com/centreon/broker/neb/statistics/generator.hh"
     *  @brief Generate engine statistics.
     */
    class          generator {
    public:
                   generator();
                   generator(generator const& right);
                   ~generator();
      generator&   operator=(generator const& right);
      void         add(
                     unsigned int host_id,
                     unsigned int service_id,
                     misc::shared_ptr<plugin> plugin);
      void         add(
                     unsigned int host_id,
                     unsigned int service_id,
                     std::string const& name);
      void         clear();
      unsigned int interval() const throw ();
      void         interval(unsigned int value);
      void         remove(
                     unsigned int host_id,
                     unsigned int service_id);
      void         run();
      void         set(config::state const& config);

    private:
      unsigned int _interval;
      std::map<std::string, misc::shared_ptr<plugin> >
                   _plugins;
      std::map<
             std::pair<unsigned int, unsigned int>,
             misc::shared_ptr<plugin> >
                   _registers;
    };
  }
}

CCB_END()

#endif // !CCB_NEB_STATISTICS_GENERATOR_HH
