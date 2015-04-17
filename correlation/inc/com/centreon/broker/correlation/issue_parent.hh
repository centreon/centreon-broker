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
**
** For more information: contact@centreon.com
*/

#ifndef CCB_CORRELATION_ISSUE_PARENT_HH
#  define CCB_CORRELATION_ISSUE_PARENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class issue_parent issue_parent.hh "com/centreon/broker/correlation/issue_parent.hh"
   *  @brief Issue parenting.
   *
   *  Declare an issue parent of another issue.
   */
  class            issue_parent : public io::data {
  public:
                   issue_parent();
                   issue_parent(issue_parent const& ip);
                   ~issue_parent();
    issue_parent&  operator=(issue_parent const& ip);
    unsigned int   type() const;
    static unsigned int
                   static_type();

    unsigned int   child_host_id;
    unsigned int   child_service_id;
    timestamp      child_start_time;
    timestamp      end_time;
    unsigned int   parent_host_id;
    unsigned int   parent_service_id;
    timestamp      parent_start_time;
    timestamp      start_time;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(issue_parent const& ip);
  };
}

CCB_END()

#endif // !CCB_CORRELATION_ISSUE_PARENT_HH
