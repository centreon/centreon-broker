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

#ifndef CCB_CORRELATION_ISSUE_HH
#define CCB_CORRELATION_ISSUE_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace correlation {
/**
 *  @class issue issue.hh "com/centreon/broker/correlation/issue.hh"
 *  @brief Issue event.
 *
 *  Update or create an issue.
 */
class issue : public io::data {
 public:
  issue();
  issue(issue const& i);
  ~issue();
  issue& operator=(issue const& i);
  bool operator==(issue const& i) const;
  bool operator!=(issue const& i) const;
  uint32_t type() const;
  static uint32_t static_type();

  timestamp ack_time;
  timestamp end_time;
  uint32_t host_id;
  uint32_t service_id;
  timestamp start_time;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(issue const& i);
};
}  // namespace correlation

CCB_END()

#endif  // !CCB_CORRELATION_ISSUE_HH
