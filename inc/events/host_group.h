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

#ifndef EVENTS_HOST_GROUP_H_
# define EVENTS_HOST_GROUP_H_

# include "events/group.h"

namespace      Events
{
  /**
   *  \class HostGroup host_group.h "events/host_group.h"
   *  \brief Represents a group of hosts within Nagios.
   *
   *  A group of hosts within Nagios.
   */
  class        HostGroup : public Group
  {
   public:
               HostGroup();
               HostGroup(const HostGroup& host_group);
               ~HostGroup();
    HostGroup& operator=(const HostGroup& host_group);
    int        GetType() const;
  };
}

#endif /* !EVENTS_HOST_GROUP_H_ */
