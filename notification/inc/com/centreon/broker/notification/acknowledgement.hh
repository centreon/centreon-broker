/*
** Copyright 2009-2012,2015 Merethis
**
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
*/

#ifndef CCB_NOTIFICATION_ACKNOWLEDGEMENT_HH
#  define CCB_NOTIFICATION_ACKNOWLEDGEMENT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/io/events.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/internal.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace            notification {
  /**
   *  @class acknowledgement acknowledgement.hh "com/centreon/broker/notification/acknowledgement.hh"
   *  @brief Represents an acknowledgement inside Nagios.
   *
   *  When some service or host is critical, Nagios will emit
   *  notifications according to its configuration. To stop the
   *  notification process, a user can acknowledge the problem.
   */
  class              acknowledgement : public io::data {
  public:
                     acknowledgement();
                     acknowledgement(acknowledgement const& other);
                     ~acknowledgement();
    acknowledgement& operator=(acknowledgement const& other);
    unsigned int     type() const;

    /**
     *  Get the type of this event.
     *
     *  @return  The event type.
     */
    static unsigned int
                     static_type() {
      return (io::events::data_type<
                            io::events::notification,
                            notification::de_acknowledgement>::value);
    }

    short            acknowledgement_type;
    QString          author;
    QString          comment;
    timestamp        deletion_time;
    timestamp        entry_time;
    unsigned int     host_id;
    bool             is_sticky;
    bool             notify_contacts;
    bool             persistent_comment;
    unsigned int     service_id;
    short            state;

    static mapping::entry const
                     entries[];
    static io::event_info::event_operations const
                     operations;

  private:
    void             _internal_copy(acknowledgement const& other);
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_ACKNOWLEDGEMENT_HH
