/*
** Copyright 2009-2010 MERETHIS
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

#ifndef EVENTS_ISSUE_PARENT_HH_
# define EVENTS_ISSUE_PARENT_HH_

# include <time.h>
# include "events/event.hh"

namespace         events {
  /**
   *  @class issue_parent issue_parent.hh "events/issue_parent.hh"
   *  @brief Issue parenting.
   *
   *  Declare an issue parent of another issue.
   */
  class           issue_parent : public event {
   private:
    void          _internal_copy(issue_parent const& ip);

   public:
    int           child_host_id;
    int           child_service_id;
    time_t        child_start_time;
    time_t        end_time;
    int           parent_host_id;
    int           parent_service_id;
    time_t        parent_start_time;
    time_t        start_time;
                  issue_parent();
                  issue_parent(issue_parent const& ip);
                  ~issue_parent();
    issue_parent& operator=(issue_parent const& ip);
    int           get_type() const;
  };
}

#endif /* !EVENTS_ISSUE_PARENT_HH_ */
