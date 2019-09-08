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

#ifndef CCB_BAM_BOOL_CONSTANT_HH
#define CCB_BAM_BOOL_CONSTANT_HH

#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_and bool_and.hh "com/centreon/broker/bam/bool_and.hh"
 *  @brief AND operator.
 *
 *  In the context of a KPI computation, bool_constant represents a constant
 *  value (i.e '42').
 */
class bool_constant : public bool_value {
 public:
  bool_constant(double value);
  bool_constant(bool_constant const& right);
  ~bool_constant();
  bool_constant& operator=(bool_constant const& right);
  bool child_has_update(computable* child, io::stream* visitor);
  double value_hard();
  double value_soft();
  bool state_known() const;

 private:
  double _value;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_CONSTANT_HH
