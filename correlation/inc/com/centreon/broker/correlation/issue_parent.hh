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
**
** For more information: contact@centreon.com
*/

#ifndef CCB_CORRELATION_ISSUE_PARENT_HH_
# define CCB_CORRELATION_ISSUE_PARENT_HH_

# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          correlation {
        /**
         *  @class issue_parent issue_parent.hh "com/centreon/broker/correlation/issue_parent.hh"
         *  @brief Issue parenting.
         *
         *  Declare an issue parent of another issue.
         */
        class            issue_parent : public io::data {
         private:
          void           _internal_copy(issue_parent const& ip);

         public:
          unsigned int   child_host_id;
          unsigned int   child_service_id;
          time_t         child_start_time;
          time_t         end_time;
          unsigned int   parent_host_id;
          unsigned int   parent_service_id;
          time_t         parent_start_time;
          time_t         start_time;
                         issue_parent();
                         issue_parent(issue_parent const& ip);
                         ~issue_parent();
          issue_parent&  operator=(issue_parent const& ip);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_CORRELATION_ISSUE_PARENT_HH_ */
