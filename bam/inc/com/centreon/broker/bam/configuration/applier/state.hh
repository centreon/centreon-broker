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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_STATE_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_STATE_HH

#  include "com/centreon/broker/bam/configuration/applier/ba.hh"
#  include "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
#  include "com/centreon/broker/bam/configuration/applier/kpi.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               bam {
  namespace             configuration {
    // Forward declaration.
    class               state;

    namespace           applier {
      /**
       *  @class state state.hh "com/centreon/broker/bam/configuration/applier/state.hh"
       *  @brief Apply global state of the BAM engine.
       *
       *  Take the configuration of the BAM engine and apply it.
       */
      class             state {
      public:
                        state();
                        state(state const& other);
                        ~state();
        state&          operator=(state const& other);
        void            apply(configuration::state const& my_state);

      private:
        void            _internal_copy(state const& other);

        ba              _ba_applier;
        kpi             _kpi_applier;
        bool_expression _bool_exp_applier;
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_STATE_HH
