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

#ifndef EVENTS_ISSUE_UPDATE_H_
# define EVENTS_ISSUE_UPDATE_H_

# include "events/event.h"

namespace        Events
{
  /**
   *  \class IssueUpdate issue_update.h "events/issue_update.h"
   *  \brief Update the presentation of a particular issue.
   *
   *  This class is currently used when merging two issues.
   */
  class          IssueUpdate : public Event
  {
   private:
    void         InternalCopy(const IssueUpdate& issue_update);

   public:
    enum         Reason
    {
      UNKNOWN = 0,
      MERGE
    };
    int          host_id1;
    int          host_id2;
    int          service_id1;
    int          service_id2;
    time_t       start_time1;
    time_t       start_time2;
    int          update;
                 IssueUpdate();
                 IssueUpdate(const IssueUpdate& issue_update);
                 ~IssueUpdate();
    IssueUpdate& operator=(const IssueUpdate& issue_update);
    int          GetType() const;
  };
}

#endif /* !EVENTS_ISSUE_UPDATE_H_ */
