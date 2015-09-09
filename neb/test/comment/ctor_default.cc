/*
** Copyright 2012-2013 Centreon
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/comment.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check comment's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::comment cmmnt;

  // Check.
  return ((cmmnt.author != "")
          || (cmmnt.comment_type != 0)
          || (cmmnt.data != "")
          || (cmmnt.deletion_time != 0)
          || (cmmnt.entry_time != 0)
          || (cmmnt.entry_type != 0)
          || (cmmnt.expire_time != 0)
          || (cmmnt.expires != false)
          || (cmmnt.host_id != 0)
          || (cmmnt.instance_id != 0)
          || (cmmnt.internal_id != 0)
          || (cmmnt.persistent != false)
          || (cmmnt.service_id != 0)
          || (cmmnt.source != 0)
          || (cmmnt.type()
              != io::events::data_type<io::events::neb, neb::de_comment>::value));
}
