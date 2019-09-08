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

#ifndef CCB_NEB_HOST_GROUP_MEMBER_HH
#define CCB_NEB_HOST_GROUP_MEMBER_HH

#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/group_member.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class host_group_member host_group_member.hh
 * "com/centreon/broker/neb/host_group_member.hh"
 *  @brief Member of a host group.
 *
 *  Base class defining that an host is part of a host group.
 *
 *  @see host
 *  @see host_group
 */
class host_group_member : public group_member {
 public:
  host_group_member();
  host_group_member(host_group_member const& other);
  virtual ~host_group_member();
  host_group_member& operator=(host_group_member const& other);
  unsigned int type() const;
  static unsigned int static_type();

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_HOST_GROUP_MEMBER_HH
