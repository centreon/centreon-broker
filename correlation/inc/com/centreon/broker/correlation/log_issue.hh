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

#ifndef CCB_CORRELATION_LOG_ISSUE_HH
#  define CCB_CORRELATION_LOG_ISSUE_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class log_issue log_issue.hh "com/centreon/broker/correlation/log_issue.hh"
   *  @brief Log issue event.
   *
   *  Link a log and an issue.
   */
  class            log_issue : public io::data {
  public:
                   log_issue();
                   log_issue(log_issue const& i);
                   ~log_issue();
    log_issue&     operator=(log_issue const& i);
    bool           operator==(log_issue const& i) const;
    bool           operator!=(log_issue const& i) const;
    unsigned int   type() const;
    static unsigned int
                   static_type();

    timestamp      log_ctime;
    unsigned int   host_id;
    unsigned int   service_id;
    timestamp      issue_start_time;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(log_issue const& i);
  };
}

CCB_END()

#endif // !CCB_CORRELATION_LOG_ISSUE_HH
