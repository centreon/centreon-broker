/*
** Copyright 2014-2016 Centreon
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

#ifndef CCB_BAM_BOOL_BINARY_OPERATOR_HH
#define CCB_BAM_BOOL_BINARY_OPERATOR_HH

#include <memory>

#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

#define COMPARE_EPSILON 0.0001

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_binary_operator bool_binary_operator.hh
 * "com/centreon/broker/bam/bool_binary_operator.hh"
 *  @brief Abstracts a binary boolean operator (AND, OR, XOR).
 *
 *  Provides common methods to binary operators.
 */
class bool_binary_operator : public bool_value {
 public:
  typedef std::shared_ptr<bool_binary_operator> ptr;

  bool_binary_operator();
  bool_binary_operator(bool_binary_operator const& right);
  virtual ~bool_binary_operator();
  bool_binary_operator& operator=(bool_binary_operator const& right);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  void set_left(std::shared_ptr<bool_value> const& left);
  void set_right(std::shared_ptr<bool_value> const& right);
  bool state_known() const;
  bool in_downtime() const;

 protected:
  std::shared_ptr<bool_value> _left;
  double _left_hard;
  double _left_soft;
  std::shared_ptr<bool_value> _right;
  double _right_hard;
  double _right_soft;
  bool _state_known;
  bool _in_downtime;

 private:
  void _internal_copy(bool_binary_operator const& right);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_BINARY_OPERATOR_HH
