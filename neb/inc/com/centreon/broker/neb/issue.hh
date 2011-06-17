/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef EVENTS_ISSUE_HH_
# define EVENTS_ISSUE_HH_

# include <sys/types.h>
# include "events/event.hh"

namespace  events {
  /**
   *  @class issue issue.hh "events/issue.hh"
   *  @brief Issue event.
   *
   *  Update or create an issue.
   */
  class    issue : public event {
   private:
    void   _internal_copy(issue const& i);

   public:
    time_t ack_time;
    time_t end_time;
    int    host_id;
    int    service_id;
    time_t start_time;
           issue();
           issue(issue const& i);
           ~issue();
    issue& operator=(issue const& i);
    int    get_type() const;
  };
}

#endif /* !EVENTS_ISSUE_HH_ */
