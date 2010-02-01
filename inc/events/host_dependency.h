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

#ifndef EVENTS_HOST_DEPENDENCY_H_
# define EVENTS_HOST_DEPENDENCY_H_

# include "events/dependency.h"

namespace           Events
{
  /**
   *  \class HostDependency host_dependency.h "events/host_dependency.h"
   *  \brief Host dependency relationship.
   *
   *  Defines a dependency between two hosts.
   */
  class             HostDependency : public Dependency
  {
   public:
                    HostDependency();
                    HostDependency(const HostDependency& host_dependency);
                    ~HostDependency();
    HostDependency& operator=(const HostDependency& host_dependency);
    int             GetType() const;
  };
}

#endif /* !EVENTS_HOST_DEPENDENCY_H_ */
