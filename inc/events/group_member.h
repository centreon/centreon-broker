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

#ifndef EVENTS_GROUP_MEMBER_H_
# define EVENTS_GROUP_MEMBER_H_

# include <string>
# include "events/event.h"

namespace        Events
{
  /**
   *  \class GroupMember group_member.h "events/group_member.h"
   *  \brief Member of a group.
   *
   *  Base class defining that a member is part of a group.
   *
   *  \see HostGroupMember
   *  \see ServiceGroupMember
   */
  class          GroupMember : public Event
  {
   private:
    void         InternalCopy(const GroupMember& gm);

   public:
    std::string  group;
    int          host_id;
    int          instance_id;
                 GroupMember();
                 GroupMember(const GroupMember& gm);
    virtual      ~GroupMember();
    GroupMember& operator=(const GroupMember& gm);
  };
}

#endif /* !EVENTS_GROUP_MEMBER_H_ */
