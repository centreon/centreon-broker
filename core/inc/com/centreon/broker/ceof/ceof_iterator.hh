/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_CEOF_CEOF_ITERATOR_HH
#define CCB_CEOF_CEOF_ITERATOR_HH

#include <vector>
#include "com/centreon/broker/ceof/ceof_token.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace ceof {
/**
 *  @class ceof_iterator ceof_iterator.hh
 * "com/centreon/broker/ceof/ceof_iterator.hh"
 *  @brief Centreon Engine Object File iterator.
 *
 *  This iterates over the parsed tokens of a ceof document.
 */
class ceof_iterator {
 public:
  ceof_iterator();
  ceof_iterator(std::vector<ceof_token>::const_iterator const& begin,
                std::vector<ceof_token>::const_iterator const& end);
  ceof_iterator(ceof_iterator const& other);
  ceof_iterator& operator=(ceof_iterator const& other);
  ~ceof_iterator() throw();
  bool operator==(ceof_iterator const& other) const throw();
  bool operator!=(ceof_iterator const& other) const throw();
  ceof_iterator& operator++() throw();

  ceof_token::token_type get_type() const throw();
  std::string const& get_value() const throw();

  bool has_children() const throw();
  ceof_iterator enter_children() const throw();

  bool end() const throw();

 private:
  std::vector<ceof_token>::const_iterator _token_it;
  std::vector<ceof_token>::const_iterator _token_end;
};
}  // namespace ceof

CCB_END()

#endif  // !CCB_CEOF_CEOF_ITERATOR_HH
