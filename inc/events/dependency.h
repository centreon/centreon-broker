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

#ifndef EVENTS_DEPENDENCY_H_
# define EVENTS_DEPENDENCY_H_

# include <string>
# include "events/event.h"

namespace       Events
{
  /**
   *  \class Dependency dependency.h "events/dependency.h"
   *  \brief Dependency relationship.
   *
   *  Defines a dependency between two objects.
   *
   *  \see HostDependency
   *  \see ServiceDependency
   */
  class         Dependency : public Event
  {
   private:
    void        InternalCopy(const Dependency& dependency);

   public:
    std::string dependency_period;
    int         dependent_object;
    std::string execution_failure_options;
    bool        inherits_parent;
    std::string notification_failure_options;
    int         object;
                Dependency();
                Dependency(const Dependency& dependency);
    virtual     ~Dependency();
    Dependency& operator=(const Dependency& dependency);
  };
}

#endif /* !EVENTS_DEPENDENCY_H_ */
