/*
** Copyright 2013 Centreon
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

#ifndef CCB_STATS_BUILDER_HH
#define CCB_STATS_BUILDER_HH

#include <string>
#include "com/centreon/broker/io/properties.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/stats/serializer.hh"

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
  void build(serializer const& srz);
  std::string const& data() const throw();
  io::properties const& root() const throw();

 private:
  static std::string _generate_stats_for_endpoint(processing::bthread* fo,
                                                  io::properties& tree);

  std::string _data;
  io::properties _root;
};
}  // namespace stats

CCB_END()

#endif  // !CCB_STATS_BUILDER_HH
