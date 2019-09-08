/*
** Copyright 2016 Centreon
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

#ifndef CCB_BAM_EXP_TOKENIZER_HH
#define CCB_BAM_EXP_TOKENIZER_HH

#include <queue>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class exp_tokenizer exp_tokenizer.hh
 * "com/centreon/broker/bam/exp_tokenizer.hh"
 *  @brief Tokenize BAM expression.
 *
 *  Split a BAM expression in tokens.
 */
class exp_tokenizer {
 public:
  exp_tokenizer(std::string const& text);
  exp_tokenizer(exp_tokenizer const& other);
  ~exp_tokenizer();
  exp_tokenizer& operator=(exp_tokenizer const& other);
  std::string next();

 private:
  std::string _extract_token();
  std::string _extract_until(bool (exp_tokenizer::*predicate)());
  void _internal_copy(exp_tokenizer const& other);
  bool _is_right_brace();
  bool _is_space_or_right_brace();
  bool _is_special_char();
  bool _is_delimiter();
  void _skip_ws();

  std::size_t _current;
  std::size_t _next;
  std::queue<std::string> _queue;
  std::size_t _size;
  std::string _text;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_EXP_TOKENIZER_HH
