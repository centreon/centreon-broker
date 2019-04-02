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

#ifndef CCB_BAM_BOOL_EXPRESSION_HH
#  define CCB_BAM_BOOL_EXPRESSION_HH

#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            bam {
  // Forward declaration.
  class              bool_value;

  /**
   *  @class bool_expression bool_expression.hh "com/centreon/broker/bam/bool_expression.hh"
   *  @brief Boolean expression.
   *
   *  Stores and entire boolean expression made of multiple boolean
   *  operations and evaluate them to match the kpi interface.
   */
  class              bool_expression : public computable {
  public:
                     bool_expression();
                     bool_expression(bool_expression const& other);
                     ~bool_expression();
    bool_expression& operator=(bool_expression const& other);
    bool             child_has_update(
                       computable* child,
                       io::stream* visitor = NULL);
    short            get_state() const;
    bool             state_known() const;
    void             set_expression(
                       std::shared_ptr<bool_value> const& expression);
    std::shared_ptr<bool_value>
                     get_expression() const;
    void             set_id(unsigned int id);
    void             set_impact_if(bool impact_if);
    bool             in_downtime() const;

  private:
    void             _internal_copy(bool_expression const& right);

    std::shared_ptr<bool_value>
                     _expression;
    unsigned int     _id;
    bool             _impact_if;
  };
}

CCB_END()

#endif // !CCB_BAM_BOOL_EXPRESSION_HH
