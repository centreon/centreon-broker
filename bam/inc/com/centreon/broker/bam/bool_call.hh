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

#ifndef CCB_BAM_BOOL_CALL_HH
#define CCB_BAM_BOOL_CALL_HH

#include <memory>
#include <string>

#include "com/centreon/broker/bam/bool_expression.hh"
#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_call bool_call.hh "com/centreon/broker/bam/bool_call.hh"
 *  @brief Bool Call.
 *
 *  In the context of a KPI computation, bool_call represents a call
 *  to an external expression.
 */
class bool_call : public bool_value {
 public:
  typedef std::shared_ptr<bool_call> ptr;

  bool_call(std::string const& name);
  bool_call(bool_call const& right);
  ~bool_call();
  bool_call& operator=(bool_call const& right);
  double value_hard();
  double value_soft();
  bool state_known() const;
  std::string const& get_name() const;
  void set_expression(std::shared_ptr<bool_value> expression);
  bool child_has_update(computable* child, io::stream* visitor = NULL);

 private:
  std::string _name;
  std::shared_ptr<bool_value> _expression;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_CALL_HH
