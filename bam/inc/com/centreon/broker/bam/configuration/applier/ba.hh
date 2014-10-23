/*
** Copyright 2014 Merethis
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
** <http:/www.gnu.org/licenses/>.
*/

#ifndef CCB_BAM_CONFIGURATION_APPLIER_BA_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_BA_HH

#  include <map>
#  include "com/centreon/broker/bam/ba.hh"
#  include "com/centreon/broker/bam/configuration/ba.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/bam/configuration/applier/timeperiod.hh"
#  include "com/centreon/broker/bam/service_book.hh"
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
        void  apply(configuration::state::bas const& my_bas,
                    service_book& book,
                    timeperiod& tp);
        misc::shared_ptr<bam::ba>
              find_ba(unsigned int id);
        void  visit(stream* visitor);

      private:
        struct applied {
          configuration::ba         cfg;
          misc::shared_ptr<bam::ba> obj;
        };

        void  _internal_copy(ba const& right);
        misc::shared_ptr<bam::ba>
              _new_ba(configuration::ba const& cfg,
                      service_book& book,
                      timeperiod& tp);

        std::map<unsigned int, applied>
              _applied;

        void _apply_timeperiods(misc::shared_ptr<bam::ba> obj,
                                unsigned int default_tp,
                                std::vector<unsigned int> const& tps,
                                timeperiod& tp);
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_BA_HH
