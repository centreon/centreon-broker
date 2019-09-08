/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_COMPOSED_BUILDER_HH

#include <vector>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class composed_builder composed_builder.hh
 * "com/centreon/broker/notification/builders/composed_builder.hh"
 *  @tparam T The builder type for the underlying builders.
 *  @brief Base for composed builders.
 *
 *  This class provides facilities for composed builders.
 */
template <typename T>
class composed_builder : public T {
 public:
  typedef std::vector<T*> vector_type;
  typedef typename vector_type::iterator iterator;
  typedef typename vector_type::const_iterator const_iterator;

  composed_builder() {}
  virtual ~composed_builder() {}

  void push_back(T& builder) { _builders.push_back(&builder); }

  iterator begin() { return (_builders.begin()); }

  const_iterator begin() const { return (_builders.begin()); }

  iterator end() { return (_builders.end()); }

  const_iterator end() const { return (_builders.end()); }

 private:
  std::vector<T*> _builders;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_COMPOSED_BUILDER_HH
