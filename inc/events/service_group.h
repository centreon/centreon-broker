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

#ifndef EVENTS_SERVICE_GROUP_H_
# define EVENTS_SERVICE_GROUP_H_

# include "events/group.h"

namespace         Events
{
  /**
   *  \class ServiceGroup service_group.h "events/service_group.h"
   *  \brief Represents a group of services within Nagios.
   *
   *  Group of services within Nagios.
   */
  class           ServiceGroup : public Group
  {
   public:
                  ServiceGroup();
                  ServiceGroup(const ServiceGroup& service_group);
                  ~ServiceGroup();
    ServiceGroup& operator=(const ServiceGroup& service_group);
    int           GetType() const;
  };
}

#endif /* !EVENTS_SERVICE_GROUP_H_ */
