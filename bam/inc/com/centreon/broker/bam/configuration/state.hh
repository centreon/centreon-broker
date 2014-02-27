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

#ifndef CCB_BAM_CONFIGURATION_STATE_HH
#  define CCB_BAM_CONFIGURATION_STATE_HH

#  include <deque>
#  include "com/centreon/broker/bam/configuration/kpi.hh"
#  include "com/centreon/broker/bam/configuration/bool_expression.hh"
#  include "com/centreon/broker/bam/configuration/ba.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam{
  namespace configuration{
    /**
     *  @class state state.hh "com/centreon/broker/bam/configuration/state.hh"
     *  @brief Configuration state.
     *
     *  Holds the entire configuration of the BAM module.
     */
    class state{
    public:
      /* Typedefs */
      typedef std::deque<ba>              bas;
      typedef std::deque<kpi>             kpis;
      typedef std::deque<bool_expression> bool_exps;

                       state();
                       state(state const& right);
                       ~state();
      state&           operator=(state const& right);

      bas const&       get_bas() const;
      kpis const&      get_kpis() const;
      bool_exps const& get_boolexps() const;

      bas&             get_bas();
      kpis&            get_kpis();
      bool_exps&       get_boolexps();

    
    private:
      bas       _bas;
      kpis      _kpis;
      bool_exps _bool_expressions;
    };
  }
}

CCB_END()

#endif // ! CCB_BAM_CONFIGURATION_STATE_HH
