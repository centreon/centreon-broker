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

#ifndef CCB_BAM_BOOL_VALUE_HH
#define CCB_BAM_BOOL_VALUE_HH

#include <memory>

#include "com/centreon/broker/bam/computable.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_value bool_value.hh "com/centreon/broker/bam/bool_value.hh"
 *  @brief Computable boolean value.
 *
 *  This class abstracts a boolean value that can get computed.
 */
class bool_value : public computable {
 public:
  typedef std::shared_ptr<bool_value> ptr;

  bool_value();
  bool_value(bool_value const& right);
  virtual ~bool_value();
  bool_value& operator=(bool_value const& right);
  virtual double value_hard() = 0;
  virtual double value_soft() = 0;
  virtual bool state_known() const = 0;
  virtual bool in_downtime() const;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_VALUE_HH
