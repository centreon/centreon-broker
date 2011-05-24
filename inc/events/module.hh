/*
** Copyright 2009-2011 MERETHIS
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

#ifndef CCB_EVENTS_MODULE_HH_
# define CCB_EVENTS_MODULE_HH_

# include <QString>
# include "events/event.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   events {
        /**
         *  @class module module.hh "events/module.hh"
         *  @brief Represents a module loaded in a Nagios instance.
         *
         *  The scheduling engine supports modules that extend its
         *  original features. This class describes such modules.
         */
        class     module : public event {
         private:
          void    _internal_copy(module const& m);

         public:
          QString args;
          QString filename;
          int     instance_id;
          bool    loaded;
          bool    should_be_loaded;
                  module();
                  module(module const& m);
                  ~module();
          module& operator=(module const& m);
          int     get_type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_MODULE_HH_ */
