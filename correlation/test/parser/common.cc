/*
** Copyright 2011-2013,2015 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "test/parser/common.hh"

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
    throw (exceptions::msg() << "state #1 has " << n1.size()
           << " elements, state #2 has " << n2.size());
  else {
    for (QMap<QPair<unsigned int, unsigned int>, correlation::node>::const_iterator
           it1(n1.begin()),
           end1(n1.end()),
           it2(n2.begin());
         it1 != end1;
         ++it1, ++it2)
      if (it1.key() != it2.key())
        throw (exceptions::msg() << "state mismatch: got key (host id "
               << it1.key().first << ", service id " << it1.key().second
               << ") against (" << it2.key().first << ", "
               << it2.key().second << ")");
      else if (*it1 != *it2)
        throw (exceptions::msg()
               << "state mismatch: got node state (host id "
               << it1->host_id << ", service id " << it1->service_id
               << ", start time " << it1->start_time << ", end time "
               << it1->end_time << ", ack time " << it1->ack_time
               << ", state " << it1->current_state << ", in downtime "
               << it1->in_downtime << ", poller id" << it1->poller_id
               << "), expected (" << it2->host_id << ", "
               << it2->service_id << ", " << it2->start_time << ", "
               << it2->end_time << ", " << it2->ack_time << ", "
               << it2->current_state << ", " << it2->in_downtime << ", "
               << it2->poller_id << ")");
  }
  return ;
}
