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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_BOOL_EXPRESSION_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_BOOL_EXPRESSION_HH

#  include <map>
#  include "com/centreon/broker/bam/configuration/bool_expression.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bam {
  // Forward declaration.
  class                  bool_expression;

  namespace              configuration {
    namespace            applier {
      // Forward declaration.
      class              ba;

      /**
       *  @class bool_expression bool_expression.hh "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
       *  @brief Apply boolean expressions.
       *
       *  Create boolean expression objects.
       */
      class              bool_expression {
      public:
                         bool_expression();
                         bool_expression(bool_expression const& other);
                         ~bool_expression();
        bool_expression& operator=(bool_expression const& other);
        void             apply(
                           configuration::state::bool_exps const& my_bools,
                           ba& my_bas);

      private:
        struct applied {
          configuration::bool_expression         cfg;
          misc::shared_ptr<bam::bool_expression> obj;
        };

        void             _internal_copy(bool_expression const& other);
        misc::shared_ptr<bam::bool_expression>
                         _new_bool_exp(
                           configuration::bool_expression const& cfg);

        std::map<unsigned int, applied>
                         _applied;
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_BOOL_EXPRESSION_HH
