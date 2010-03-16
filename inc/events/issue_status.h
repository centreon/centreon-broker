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

#ifndef EVENTS_ISSUE_STATUS_H_
# define EVENTS_ISSUE_STATUS_H_

# include <string>
# include <time.h>
# include "events/event.h"

namespace        Events
{
  /**
   *  \class IssueStatus issue_status.h "events/issue_status.h"
   *  \brief Update an already existing issue.
   *
   *  Update status of an issue, which is itself a group of Nagios events.
   *
   *  \see Issue
   */
  class          IssueStatus : public Event
  {
   private:
    void         InternalCopy(const IssueStatus& issue_status);

   public:
    time_t       ack_time;
    int          host_id;
    std::string  output;
    int          service_id;
    time_t       start_time;
    short        state;
    short        status;
                 IssueStatus();
                 IssueStatus(const IssueStatus& issue_status);
    virtual      ~IssueStatus();
    IssueStatus& operator=(const IssueStatus& issue_status);
    virtual int  GetType() const;
  };
}

#endif /* !EVENTS_ISSUE_STATUS_H_ */
