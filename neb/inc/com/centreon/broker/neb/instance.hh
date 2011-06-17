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

#ifndef CCB_NEB_INSTANCE_HH_
# define CCB_NEB_INSTANCE_HH_

# include <QString>
# include <time.h>
# include "com/centreon/broker/io/data.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          neb {
        /**
         *  @class instance instance.hh "com/centreon/broker/neb/instance.hh"
         *  @brief Information about Nagios process.
         *
         *  This class holds information about a Nagios process, like whether
         *  it is running or not, in daemon mode or not, ...
         */
        class            instance : public io::data {
         private:
          void           _internal_copy(instance const& i);

         public:
          QString        engine;
          unsigned int   id;
          bool           is_running;
          QString        name;
          unsigned int   pid;
          time_t         program_end;
          time_t         program_start;
          QString        version;
                         instance();
                         instance(instance const& i);
                         ~instance();
          instance&      operator=(instance const& i);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_INSTANCE_HH_ */
