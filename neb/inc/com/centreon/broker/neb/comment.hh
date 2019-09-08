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

#ifndef CCB_NEB_COMMENT_HH
#define CCB_NEB_COMMENT_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class comment comment.hh "com/centreon/broker/neb/comment.hh"
 *  @brief Represents a comment inside Nagios.
 *
 *  Some user can make a comment on whatever objects he wants.
 */
class comment : public io::data {
 public:
  comment();
  comment(comment const& other);
  ~comment();
  comment& operator=(comment const& other);
  unsigned int type() const;
  static unsigned int static_type();

  std::string author;
  short comment_type;
  std::string data;
  timestamp deletion_time;
  timestamp entry_time;
  short entry_type;
  timestamp expire_time;
  bool expires;
  unsigned int host_id;
  unsigned int internal_id;
  bool persistent;
  unsigned int poller_id;
  unsigned int service_id;
  short source;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(comment const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_COMMENT_HH
