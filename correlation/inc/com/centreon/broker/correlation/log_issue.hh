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

#ifndef CCB_CORRELATION_LOG_ISSUE_HH
#define CCB_CORRELATION_LOG_ISSUE_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace correlation {
/**
 *  @class log_issue log_issue.hh "com/centreon/broker/correlation/log_issue.hh"
 *  @brief Log issue event.
 *
 *  Link a log and an issue.
 */
class log_issue : public io::data {
 public:
  log_issue();
  log_issue(log_issue const& i);
  ~log_issue();
  log_issue& operator=(log_issue const& i);
  bool operator==(log_issue const& i) const;
  bool operator!=(log_issue const& i) const;
  unsigned int type() const;
  static unsigned int static_type();

  timestamp log_ctime;
  unsigned int host_id;
  unsigned int service_id;
  timestamp issue_start_time;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(log_issue const& i);
};
}  // namespace correlation

CCB_END()

#endif  // !CCB_CORRELATION_LOG_ISSUE_HH
