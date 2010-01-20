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

#ifndef EVENTS_SERVICE_GROUP_MEMBER_H_
# define EVENTS_SERVICE_GROUP_MEMBER_H_

# include <string>
# include "events/group_member.h"

namespace               Events
{
  /**
   *  \class ServiceGroupMember service_group_member.h "events/service_group_member.h"
   *  \brief Member of a service group.
   *
   *  Base class defining that a service is part of a service group.
   *
   *  \see Service
   *  \see ServiceGroup
   */
  class                 ServiceGroupMember : public GroupMember
  {
   public:
                        ServiceGroupMember();
                        ServiceGroupMember(const ServiceGroupMember& sgm);
    virtual             ~ServiceGroupMember();
    ServiceGroupMember& operator=(const ServiceGroupMember& sgm);
    int                 GetType() const;
  };
}

#endif /* !EVENTS_SERVICE_GROUP_MEMBER_H_ */
