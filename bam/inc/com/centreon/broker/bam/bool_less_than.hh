/*
** Copyright 2014, 2021 Centreon
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

#ifndef CCB_BAM_BOOL_LESS_THAN_HH
#define CCB_BAM_BOOL_LESS_THAN_HH

#include "com/centreon/broker/bam/bool_binary_operator.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_less_than bool_less_than.hh
 * "com/centreon/broker/bam/bool_less_than.hh"
 *  @brief Less than operator.
 *
 *  In the context of a KPI computation, bool_les_than represents a logical
 *  less than between two bool_value.
 */
class bool_less_than : public bool_binary_operator {
  const bool _strict;

 public:
  bool_less_than(bool strict = false);
  ~bool_less_than() noexcept = default;
  bool_less_than(bool_less_than const&) = delete;
  bool_less_than& operator=(bool_less_than const&) = delete;
  double value_hard();
  double value_soft();
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_LESS_THAN_HH
