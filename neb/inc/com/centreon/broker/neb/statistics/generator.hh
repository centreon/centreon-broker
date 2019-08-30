/*
** Copyright 2013 Centreon
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

#ifndef CCB_NEB_STATISTICS_GENERATOR_HH
#  define CCB_NEB_STATISTICS_GENERATOR_HH

#  include <map>
#  include <memory>
#  include <string>
#  include <utility>
#  include "com/centreon/broker/config/state.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/statistics/plugin.hh"

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
                     std::shared_ptr<plugin> plugin);
      void         add(
                     unsigned int host_id,
                     unsigned int service_id,
                     std::string const& name);
      void         clear();
      unsigned int interval() const throw ();
      void         interval(unsigned int value);
      void         remove(
                     uint64_t host_id,
                     uint64_t service_id);
      void         run();
      void         set(config::state const& config);

    private:
      unsigned int _interval;
      std::map<std::string, std::shared_ptr<plugin> >
                   _plugins;
      std::map<std::pair<uint64_t, uint64_t>, std::shared_ptr<plugin>>
          _registers;
    };
  }
}

CCB_END()

#endif // !CCB_NEB_STATISTICS_GENERATOR_HH
