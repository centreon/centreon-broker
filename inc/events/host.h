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

#ifndef EVENTS_HOST_H_
# define EVENTS_HOST_H_

# include <string>
# include "events/host_service.h"
# include "events/host_status.h"

namespace              Events
{
  /**
   *  \class Host host.h "events/host.h"
   *  \brief Host within Nagios.
   *
   *  Nagios has two main objects that can be manipulated : host and service.
   *  An Host object holds every parameter related to a host machine.
   */
  class                Host : public HostService, public HostStatus
  {
   private:
    void               InternalCopy(const Host& h);
    void               ZeroInitialize();

   public:
    std::string        address;
    std::string        alias;
    bool               flap_detection_on_down;
    bool               flap_detection_on_unreachable;
    bool               flap_detection_on_up;
    bool               notify_on_down;
    bool               notify_on_unreachable;
    bool               stalk_on_down;
    bool               stalk_on_unreachable;
    bool               stalk_on_up;
    std::string        statusmap_image;
                       Host();
                       Host(const HostStatus& hs);
                       Host(const Host& h);
                       ~Host();
    Host&              operator=(const Host& h);
    int                GetType() const;
  };
}

#endif /* !EVENTS_HOST_H_ */
