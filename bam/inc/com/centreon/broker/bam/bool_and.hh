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

#ifndef CCB_BAM_BOOL_AND_HH
#define CCB_BAM_BOOL_AND_HH

#include "com/centreon/broker/bam/bool_binary_operator.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_and bool_and.hh "com/centreon/broker/bam/bool_and.hh"
 *  @brief AND operator.
 *
 *  In the context of a KPI computation, bool_and represents a logical
 *  AND between two bool_value.
 */
class bool_and : public bool_binary_operator {
 public:
  bool_and() = default;
  ~bool_and() noexcept = default;
  bool_and(const bool_and&) = delete;
  bool_and& operator=(const bool_and&) = delete;
  double value_hard() override;
  double value_soft() override;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_AND_HH
