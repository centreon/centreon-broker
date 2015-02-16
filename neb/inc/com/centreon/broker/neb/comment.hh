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

#ifndef CCB_NEB_COMMENT_HH
#  define CCB_NEB_COMMENT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace          neb {
  /**
   *  @class comment comment.hh "com/centreon/broker/neb/comment.hh"
   *  @brief Represents a comment inside Nagios.
   *
   *  Some user can make a comment on whatever objects he wants.
   */
  class            comment : public io::data {
  public:
                   comment();
                   comment(comment const& other);
                   ~comment();
    comment&       operator=(comment const& other);
    unsigned int   type() const;

    QString        author;
    short          comment_type;
    QString        data;
    timestamp      deletion_time;
    timestamp      entry_time;
    short          entry_type;
    timestamp      expire_time;
    bool           expires;
    unsigned int   host_id;
    unsigned int   internal_id;
    bool           persistent;
    unsigned int   service_id;
    short          source;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(comment const& other);
  };
}

CCB_END()

#endif // !CCB_NEB_COMMENT_HH
