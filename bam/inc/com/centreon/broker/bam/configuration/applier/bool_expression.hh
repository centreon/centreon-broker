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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_BOOL_EXPRESSION_HH
#  define CCB_BAM_CONFIGURATION_APPLIER_BOOL_EXPRESSION_HH

#  include <list>
#  include <map>
#  include <memory>
#  include "com/centreon/broker/bam/configuration/bool_expression.hh"
#  include "com/centreon/broker/bam/configuration/state.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bam {
  // Forward declarations.
  class                  bool_expression;
  class                  bool_service;
  class                  bool_call;
  class                  bool_metric;
  class                  service_book;
  class                  metric_book;

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
                           hst_svc_mapping const& mapping,
                           service_book& book,
                           metric_book& metric_book);
        std::shared_ptr<bam::bool_expression>
                         find_boolexp(unsigned int id);

      private:
        struct applied {
          configuration::bool_expression                  cfg;
          std::shared_ptr<bam::bool_expression>           obj;
          std::list<std::shared_ptr<bam::bool_service>>   svc;
          std::list<std::shared_ptr<bam::bool_call>>      call;
          std::list<std::shared_ptr<bam::bool_metric>>    mtrc;
        };

        void             _internal_copy(bool_expression const& other);
        std::shared_ptr<bam::bool_expression>
                         _new_bool_exp(
                           configuration::bool_expression const& cfg);
        void             _resolve_expression_calls();

        std::map<unsigned int, applied>
                         _applied;
      };
    }
  }
}

CCB_END()

#endif // !CCB_BAM_CONFIGURATION_APPLIER_BOOL_EXPRESSION_HH