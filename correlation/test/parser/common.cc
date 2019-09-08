/*
** Copyright 2011-2013 Centreon
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

#include "test/parser/common.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

/**
 *  Compare two states.
 *
 *  @param[in] n1 First state.
 *  @param[in] n2 Second state.
 */
void compare_states(
    QMap<QPair<unsigned int, unsigned int>, correlation::node> const& n1,
    QMap<QPair<unsigned int, unsigned int>, correlation::node> const& n2) {
  if (n1.size() != n2.size())
    throw(exceptions::msg() << "state #1 has " << n1.size()
                            << " elements, state #2 has " << n2.size());
  else {
    for (QMap<QPair<unsigned int, unsigned int>,
              correlation::node>::const_iterator it1(n1.begin()),
         end1(n1.end()), it2(n2.begin());
         it1 != end1; ++it1, ++it2)
      if (it1.key() != it2.key())
        throw(exceptions::msg()
              << "state mismatch: got key (host id " << it1.key().first
              << ", service id " << it1.key().second << ") against ("
              << it2.key().first << ", " << it2.key().second << ")");
      else if (*it1 != *it2)
        throw(exceptions::msg()
              << "state mismatch: got node state (host id " << it1->host_id
              << ", service id " << it1->service_id << ", start time "
              << it1->start_time << ", end time " << it1->end_time
              << ", ack time " << it1->ack_time << ", state "
              << it1->current_state << ", in downtime " << it1->in_downtime
              << ", poller id" << it1->poller_id << "), expected ("
              << it2->host_id << ", " << it2->service_id << ", "
              << it2->start_time << ", " << it2->end_time << ", "
              << it2->ack_time << ", " << it2->current_state << ", "
              << it2->in_downtime << ", " << it2->poller_id << ")");
  }
  return;
}
