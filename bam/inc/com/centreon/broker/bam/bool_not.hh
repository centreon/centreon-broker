/*
** Copyright 2014,2016 Centreon
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

#ifndef CCB_BAM_BOOL_NOT_HH
#define CCB_BAM_BOOL_NOT_HH

#include <memory>

#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
// Forward declaration.
class bool_value;

/**
 *  @class bool_not bool_not.hh "com/centreon/broker/bam/bool_not.hh"
 *  @brief NOT boolean operator.
 *
 *  In the context of a KPI computation, bool_not represents a logical
 *  NOT on a bool_value.
 */
class bool_not : public bool_value {
 public:
  bool_not(bool_value::ptr val = bool_value::ptr());
  bool_not(bool_not const& right);
  ~bool_not();
  bool_not& operator=(bool_not const& right);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  void set_value(std::shared_ptr<bool_value>& value);
  double value_hard();
  double value_soft();
  bool state_known() const;
  bool in_downtime() const;

 private:
  void _internal_copy(bool_not const& right);

  bool_value::ptr _value;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_NOT_HH
