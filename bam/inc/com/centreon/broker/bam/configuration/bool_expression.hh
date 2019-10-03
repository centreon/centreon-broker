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

#ifndef BAM_CCB_CONFIGURATION_BOOL_EXPRESSION_HH
#define BAM_CCB_CONFIGURATION_BOOL_EXPRESSION_HH

#include <list>
#include <string>
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
namespace configuration {
/**
 *  @class bool_expression bool_expression.hh
 * "com/centreon/broker/bam/configuration/bool_expression.h"
 *  @brief A pseudo-kpi triggered by a the value of a boolean expression.
 *
 *  Configuration of a boolean expression, later used to impact one
 *  or more BA.
 */
class bool_expression {
 public:
  bool_expression(uint32_t id = 0,
                  std::string const& name = "",
                  std::string const& expression = "",
                  bool impact_if = false);
  bool_expression(bool_expression const& other);
  ~bool_expression();
  bool_expression& operator=(bool_expression const& other);
  bool operator==(bool_expression const& other) const;
  bool operator!=(bool_expression const& other) const;

  uint32_t get_id() const;
  std::string const& get_name() const;
  std::string const& get_expression() const;
  bool get_impact_if() const;

  void set_name(std::string const& name);
  void set_expression(std::string const& s);
  void set_id(uint32_t id);
  void set_impact_if(bool b);

 private:
  uint32_t _id;
  std::string _name;
  std::string _expression;
  bool _impact_if;
};
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_BOOL_EXPRESSION_HH
