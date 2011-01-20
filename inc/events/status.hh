/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#ifndef EVENTS_STATUS_HH_
# define EVENTS_STATUS_HH_

# include "events/event.hh"

namespace       events {
  /**
   *  @class status status.hh "events/status.hh"
   *  @brief Root class of status events.
   *
   *  This is the root class of status events : host, program and
   *  service status events.
   *
   *  @see host_status
   *  @see program_status
   *  @see service_status
   */
  class         status : public event {
   private:
    void        _internal_copy(status const& s);

   public:
    bool        event_handler_enabled;
    bool        failure_prediction_enabled;
    bool        flap_detection_enabled;
    bool        notifications_enabled;
    bool        process_performance_data;
                status();
                status(status const& s);
    virtual     ~status();
    status&     operator=(status const& s);
  };
}

#endif /* !EVENTS_STATUS_HH_ */
