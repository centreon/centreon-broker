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

#ifndef CCB_NEB_LOG_ENTRY_HH
#  define CCB_NEB_LOG_ENTRY_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class log_entry log_entry.hh "com/centreon/broker/neb/log_entry.hh"
   *  @brief Log message.
   *
   *  From time to time, the scheduling engine generates a log
   *  message. These messages can be useful, especially when
   *  investigating problems. This class holds all information
   *  related to a log entry.
   */
  class            log_entry : public io::data {
  public:
                   log_entry();
                   log_entry(log_entry const& le);
                   ~log_entry();
    log_entry&     operator=(const log_entry& le);
    unsigned int   type() const;

    timestamp      c_time;
    unsigned int   host_id;
    QString        host_name;
    unsigned int   instance_id;
    QString        instance_name;
    timestamp      issue_start_time;
    short          log_type;
    short          msg_type;
    QString        notification_cmd;
    QString        notification_contact;
    QString        output;
    int            retry;
    QString        service_description;
    unsigned int   service_id;
    short          status;

  private:
    void           _internal_copy(log_entry const& le);
  };
}

CCB_END()

#endif // !CCB_NEB_LOG_ENTRY_HH
