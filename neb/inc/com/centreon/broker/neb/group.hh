/*
** Copyright 2009-2012,2015 Centreon
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

#ifndef CCB_NEB_GROUP_HH
#define CCB_NEB_GROUP_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class group group.hh "com/centreon/broker/neb/group.hh"
 *  @brief Base of host and service group classes.
 *
 *  The scheduling engine handles group. This can be service groups
 *  or host groups for example.
 *
 *  @see host_group
 *  @see service_group
 */
class group : public io::data {
 public:
  group();
  group(group const& other);
  virtual ~group();
  group& operator=(group const& other);

  bool enabled;
  unsigned int id;
  std::string name;
  unsigned int poller_id;

 private:
  void _internal_copy(group const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_GROUP_HH
