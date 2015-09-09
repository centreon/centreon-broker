/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_BA_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_BA_HH

#  include <map>
#  include "com/centreon/broker/bam/ba.hh"
#  include "com/centreon/broker/bam/configuration/ba.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/bam/service_book.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     bam {
  namespace   configuration {
    namespace applier {
      /**
       *  @class ba ba.hh "com/centreon/broker/bam/configuration/applier/ba.hh"
       *  @brief Apply BA configuration.
       *
       *  Take the configuration of BAs and apply it.
       */
      class   ba {
      public:
              ba();
              ba(ba const& right);
              ~ba();
        ba&   operator=(ba const& right);
        void  apply(
                configuration::state::bas const& my_bas,
                service_book& book);
        misc::shared_ptr<bam::ba>
              find_ba(unsigned int id);
        void  visit(io::stream* visitor);

      private:
        struct applied {
          configuration::ba         cfg;
          misc::shared_ptr<bam::ba> obj;
        };

        void  _internal_copy(ba const& right);
        misc::shared_ptr<bam::ba>
              _new_ba(configuration::ba const& cfg,
                      service_book& book);

        std::map<unsigned int, applied>
              _applied;
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_BA_HH
