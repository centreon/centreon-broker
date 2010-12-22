/*
** Copyright 2009-2010 MERETHIS
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

#ifndef EVENTS_COMMENT_HH_
# define EVENTS_COMMENT_HH_

# include <string>
# include <sys/time.h>
# include "events/event.hh"

namespace              events {
  /**
   *  @class comment comment.hh "events/comment.hh"
   *  @brief Represents a comment inside Nagios.
   *
   *  Some user can make a comment on whatever objects he wants.
   */
  class                comment : public event {
   private:
    void               _internal_copy(comment const& c);

   public:
    std::string        author;
    std::string        data;
    time_t             deletion_time;
    time_t             entry_time;
    short              entry_type;
    time_t             expire_time;
    bool               expires;
    std::string        host_name;
    std::string        instance_name;
    int                internal_id;
    bool               persistent;
    std::string        service_description;
    short              source;
    short              type;
                       comment();
                       comment(comment const& c);
                       ~comment();
    comment&           operator=(comment const& c);
    int                get_type() const;
  };
}

#endif /* !EVENTS_COMMENT_HH_ */
