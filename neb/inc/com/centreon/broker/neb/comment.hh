/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NEB_COMMENT_HH_
# define CCB_NEB_COMMENT_HH_

# include <QString>
# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          neb {
        /**
         *  @class comment comment.hh "com/centreon/broker/neb/comment.hh"
         *  @brief Represents a comment inside Nagios.
         *
         *  Some user can make a comment on whatever objects he wants.
         */
        class            comment : public io::data {
         private:
          void           _internal_copy(comment const& c);

         public:
          QString        author;
          short          comment_type;
          QString        data;
          time_t         deletion_time;
          time_t         entry_time;
          short          entry_type;
          time_t         expire_time;
          bool           expires;
          unsigned int   host_id;
          unsigned int   instance_id;
          unsigned int   internal_id;
          bool           persistent;
          unsigned int   service_id;
          short          source;
                         comment();
                         comment(comment const& c);
                         ~comment();
          comment&       operator=(comment const& c);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_COMMENT_HH_ */
