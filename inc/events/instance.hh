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

#ifndef CCB_EVENTS_INSTANCE_HH_
# define CCB_EVENTS_INSTANCE_HH_

# include <QString>
# include <time.h>
# include "events/event.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        events {
        /**
         *  @class instance instance.hh "events/instance.hh"
         *  @brief Information about Nagios process.
         *
         *  This class holds information about a Nagios process, like whether
         *  it is running or not, in daemon mode or not, ...
         */
        class          instance : public event {
         private:
          void         _internal_copy(instance const& i);

         public:
          QString      engine;
          int          id;
          bool         is_running;
          QString      name;
          int          pid;
          time_t       program_end;
          time_t       program_start;
          QString      version;
                       instance();
                       instance(instance const& i);
                       ~instance();
          instance&    operator=(instance const& i);
          unsigned int type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_INSTANCE_HH_ */
