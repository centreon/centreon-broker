/*
** Copyright 2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/neb/comment.hh"

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
              != "com::centreon::broker::neb::comment"));
}
