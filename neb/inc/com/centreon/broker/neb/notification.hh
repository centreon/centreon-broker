/*
** Copyright 2009-2013 Merethis
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

#ifndef CCB_NEB_NOTIFICATION_HH
#  define CCB_NEB_NOTIFICATION_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class notification notification.hh "com/centreon/broker/neb/notification.hh"
   *  @brief Represents a notification.
   *
   *  Notifications are sent by the scheduling engine to notify
   *  users of an issue in their monitored IT infrastructure.
   */
  class            notification : public io::data {
  public:
                   notification();
                   notification(notification const& n);
                   ~notification();
    notification&  operator=(notification const& n);
    unsigned int   type() const;

    QString        ack_author;
    QString        ack_data;
    QString        command_name;
    QString        contact_name;
    bool           contacts_notified;
    timestamp      end_time;
    bool           escalated;
    unsigned int   host_id;
    unsigned int   instance_id;
    int            notification_type;
    QString        output;
    int            reason_type;
    unsigned int   service_id;
    timestamp      start_time;
    int            state;

  private:
    void           _internal_copy(notification const& n);
  };
}

CCB_END()

#endif // !CCB_NEB_NOTIFICATION_HH
