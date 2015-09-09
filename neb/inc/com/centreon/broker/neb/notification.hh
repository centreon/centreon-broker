/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
