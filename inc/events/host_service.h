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

#ifndef EVENTS_HOST_SERVICE_H_
# define EVENTS_HOST_SERVICE_H_

# include <string>

namespace              Events
{
  /**
   *  \class HostService host_service.h "events/host_service.h"
   *  \brief Common class to Host and Service.
   *
   *  This class holds data members common to Host and Service classes.
   *
   *  \see Host
   *  \see Service
   */
  class                HostService
  {
   private:
    void               InternalCopy(const HostService& hs);

   public:
    std::string        action_url;
    bool               check_freshness;
    std::string        display_name;
    double             first_notification_delay;
    double             freshness_threshold;
    double             high_flap_threshold;
    std::string        host;
    std::string        icon_image;
    std::string        icon_image_alt;
    int                instance_id;
    double             low_flap_threshold;
    std::string        notes;
    std::string        notes_url;
    double             notification_interval;
    std::string        notification_period;
    short              notify_on_downtime;
    short              notify_on_flapping;
    short              notify_on_recovery;
    bool               retain_nonstatus_information;
    bool               retain_status_information;
                       HostService();
                       HostService(const HostService& hs);
    virtual            ~HostService();
    HostService&       operator=(const HostService& hs);
  };
}

#endif /* !EVENTS_HOST_SERVICE_H_ */
