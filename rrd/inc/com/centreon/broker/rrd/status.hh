/*
** Copyright 2011 Merethis
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

#ifndef CCB_RRD_STATUS_HH_
# define CCB_RRD_STATUS_HH_

# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          rrd {
        /**
         *  @class status status.hh "com/centreon/broker/rrd/status.hh"
         *  @brief Status data used to generate status graphs.
         *
         *  Status data event used to generate status graphs.
         */
        class            status : public io::data {
         private:
          void           _internal_copy(status const& s);

         public:
          time_t         ctime;
          unsigned int   index_id;
          time_t         interval;
          time_t         rrd_len;
          short          state;
                         status();
                         status(status const& s);
                         ~status();
          status&        operator=(status const& s);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_RRD_STATUS_HH_ */
