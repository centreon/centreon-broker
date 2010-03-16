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

#ifndef EVENTS_ISSUE_H_
# define EVENTS_ISSUE_H_

# include <time.h>                // for time_t
# include "events/issue_status.h"

namespace       Events
{
  /**
   *  \class Issue issue.h "events/issue.h"
   *  \brief Correlate events that Nagios generated.
   *
   *  Nagios can generate multiple events and log messages that are related
   *  to the same issue on the network. This class represents such issues.
   */
  class         Issue : public IssueStatus
  {
   public:
    time_t      end_time;
                Issue();
                Issue(const Issue& issue);
                ~Issue();
    Issue&      operator=(const Issue& issue);
    int         GetType() const;
  };
}

#endif /* !EVENTS_ISSUE_H_ */
