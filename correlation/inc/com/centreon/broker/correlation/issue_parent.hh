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

#ifndef CCB_CORRELATION_ISSUE_PARENT_HH
#define CCB_CORRELATION_ISSUE_PARENT_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace correlation {
/**
 *  @class issue_parent issue_parent.hh
 * "com/centreon/broker/correlation/issue_parent.hh"
 *  @brief Issue parenting.
 *
 *  Declare an issue parent of another issue.
 */
class issue_parent : public io::data {
 public:
  issue_parent();
  issue_parent(issue_parent const& ip);
  ~issue_parent();
  issue_parent& operator=(issue_parent const& ip);
  unsigned int type() const;
  static unsigned int static_type();

  unsigned int child_host_id;
  unsigned int child_service_id;
  timestamp child_start_time;
  timestamp end_time;
  unsigned int parent_host_id;
  unsigned int parent_service_id;
  timestamp parent_start_time;
  timestamp start_time;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(issue_parent const& ip);
};
}  // namespace correlation

CCB_END()

#endif  // !CCB_CORRELATION_ISSUE_PARENT_HH
