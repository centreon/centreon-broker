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

#ifndef CCB_BAM_BOOL_OPERATION_HH
#define CCB_BAM_BOOL_OPERATION_HH

#include <string>
#include "com/centreon/broker/bam/bool_binary_operator.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_operation bool_operation.hh
 * "com/centreon/broker/bam/bool_operation.hh"
 *  @brief Boolean operation.
 *
 *  In the context of a KPI computation, bool_operation represents a
 *  mathematical operation between two bool_value.
 */
class bool_operation : public bool_binary_operator {
 public:
  bool_operation(std::string const& op);
  bool_operation(bool_operation const& right);
  ~bool_operation();
  bool_operation& operator=(bool_operation const& right);
  double value_hard();
  double value_soft();
  bool state_known() const;

 private:
  enum operation_type {
    addition,
    substraction,
    multiplication,
    division,
    modulo
  };
  operation_type _type;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_OR_HH
