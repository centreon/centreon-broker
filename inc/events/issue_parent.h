/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef EVENTS_ISSUE_PARENT_H_
# define EVENTS_ISSUE_PARENT_H_

# include <time.h>
# include "events/event.h"

namespace        Events
{
  /**
   *  @class IssueParent issue_parent.h "events/issue_parent.h"
   *  @brief Issue parenting.
   *
   *  Declare an issue parent of another issue.
   */
  class          IssueParent : public Event
  {
   private:
    void         InternalCopy(const IssueParent& issue_parent);

   public:
    int          child_host_id;
    int          child_service_id;
    time_t       child_start_time;
    time_t       end_time;
    int          parent_host_id;
    int          parent_service_id;
    time_t       parent_start_time;
    time_t       start_time;
                 IssueParent();
                 IssueParent(const IssueParent& issue_parent);
                 ~IssueParent();
    IssueParent& operator=(const IssueParent& issue_parent);
    int          GetType() const;
  };
}

#endif /* !EVENTS_ISSUE_PARENT_H_ */
