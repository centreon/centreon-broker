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

#ifndef CCB_EVENTS_PERFDATA_HH_
# define CCB_EVENTS_PERFDATA_HH_

# include <time.h>
# include "events/event.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        events {
        /**
         *  @class perfdata perfdata.hh "events/perfdata.hh"
         *  @brief Perfdata object.
         *
         *  Already processed perfdata, ready to get inserted.
         */
        class          perfdata : public event {
         private:
          void         _internal_copy(perfdata const& p);

         public:
          time_t       ctime;
          int          metric_id;
          short        status;
          double       value;
                       perfdata();
                       perfdata(perfdata const& p);
                       ~perfdata();
          perfdata&    operator=(perfdata const& p);
          unsigned int type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_PERFDATA_HH_ */
