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

#ifndef CCB_STATS_SERIALIZER_HH
#define CCB_STATS_SERIALIZER_HH

#include <map>
#include <string>
#include "com/centreon/broker/io/properties.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace stats {
/**
 *  @class serializer serializer.hh "com/centreon/broker/stats/serializer.hh"
 *  @brief Interface for serializer objects.
 */
class serializer {
 public:
  virtual ~serializer() {}

  virtual void serialize(std::string& buffer,
                         io::properties const& tree) const = 0;
};
}  // namespace stats

CCB_END()

#endif  // !CCB_STATS_SERIALIZER_HH
