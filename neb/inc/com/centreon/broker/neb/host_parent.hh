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

#ifndef CCB_NEB_HOST_PARENT_HH
#define CCB_NEB_HOST_PARENT_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class host_parent host_parent.hh "com/centreon/broker/neb/host_parent.hh"
 *  @brief Define a parent of a host.
 *
 *  Define a certain host to be the parent of another host.
 */
class host_parent : public io::data {
 public:
  host_parent();
  host_parent(host_parent const& other);
  ~host_parent();
  host_parent& operator=(host_parent const& other);
  unsigned int type() const;
  static unsigned int static_type();

  bool enabled;
  unsigned int host_id;
  unsigned int parent_id;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_HOST_PARENT_HH
