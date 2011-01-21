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

#ifndef EVENTS_ACKNOWLEDGEMENT_HH_
# define EVENTS_ACKNOWLEDGEMENT_HH_

# include <string>
# include <sys/types.h>
# include "events/event.hh"

namespace              events {
  /**
   *  @class acknowledgement acknowledgement.hh "events/acknowledgement.hh"
   *  @brief Represents an acknowledgement inside Nagios.
   *
   *  When some service or host is critical, Nagios will emit
   *  notifications according to its configuration. To stop the
   *  notification process, a user can acknowledge the problem.
   */
  class                acknowledgement : public event {
   private:
    void               _internal_copy(acknowledgement const& ack);

   public:
    short              acknowledgement_type;
    std::string        author;
    std::string        comment;
    time_t             entry_time;
    int                host_id;
    int                instance_id;
    bool               is_sticky;
    bool               notify_contacts;
    bool               persistent_comment;
    int                service_id;
    short              state;
                       acknowledgement();
                       acknowledgement(acknowledgement const& ack);
                       ~acknowledgement();
    acknowledgement&   operator=(acknowledgement const& ack);
    int                get_type() const;
  };
}

#endif /* !EVENTS_ACKNOWLEDGEMENT_HH_ */
