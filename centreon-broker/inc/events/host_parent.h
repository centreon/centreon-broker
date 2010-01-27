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

#ifndef EVENTS_HOST_PARENT_H_
# define EVENTS_HOST_PARENT_H_

# include <string>
# include "events/event.h"

namespace       Events
{
  /**
   *  \class HostParent host_parent.h "events/host_parent.h"
   *  \brief Define a parent of a host.
   *
   *  Define a certain host to be the parent of another host.
   */
  class         HostParent : public Event
  {
   public:
    std::string host;
    std::string parent;
                HostParent();
                HostParent(const HostParent& hp);
                ~HostParent();
    HostParent& operator=(const HostParent& hp);
    int         GetType() const;
  };
}

#endif /* !EVENTS_HOST_PARENT_H_ */
