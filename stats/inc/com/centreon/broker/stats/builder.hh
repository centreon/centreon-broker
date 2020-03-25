/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#ifndef CCB_STATS_BUILDER_HH
#define CCB_STATS_BUILDER_HH

#include <json11.hpp>
#include <mutex>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace processing {
class bthread;
}

namespace stats {
/**
 *  @class builder builder.hh "com/centreon/broker/stats/builder.hh"
 *  @brief Parse a <stats> node.
 */
class builder {
 public:
  builder();
  builder(builder const& right);
  ~builder() throw();
  builder& operator=(builder const& right);
  void build();
  std::string const& data() const noexcept;
  json11::Json const& root() const noexcept;


 private:
  std::string _data;
  json11::Json _root;
};
}  // namespace stats

CCB_END()

#endif  // !CCB_STATS_BUILDER_HH
