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

#ifndef CCB_NEB_GROUP_MEMBER_HH
#define CCB_NEB_GROUP_MEMBER_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class group_member group_member.h "com/centreon/broker/neb/group_member.hh"
 *  @brief Member of a group.
 *
 *  Base class defining that a member is part of a group.
 *
 *  @see host_group_member
 *  @see service_group_member
 */
class group_member : public io::data {
 public:
  group_member();
  group_member(group_member const& other);
  virtual ~group_member();
  group_member& operator=(group_member const& other);

  bool enabled;
  unsigned int group_id;
  std::string group_name;
  unsigned int host_id;
  unsigned int poller_id;

 private:
  void _internal_copy(group_member const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_GROUP_MEMBER_HH
