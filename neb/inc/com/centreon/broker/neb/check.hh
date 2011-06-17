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

#ifndef CCB_NEB_CHECK_HH_
# define CCB_NEB_CHECK_HH_

# include <QString>
# include "com/centreon/broker/io/data.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   neb {
        /**
         *  @class check check.hh "com/centreon/broker/neb/check.hh"
         *  @brief Check that has been executed.
         *
         *  Once a check has been executed (the check itself, not
         *  deduced information), this kind of event is sent.
         *
         *  @see host_check
         *  @see service_check
         */
        class          check : public io::data {
         private:
          void         _internal_copy(check const& c);

         public:
          QString      command_line;
          unsigned int host_id;
                       check();
                       check(check const& c);
          virtual      ~check();
          check&       operator=(check const& c);
        };
      }
    }
  }
}

#endif /* !CCB_NEB_CHECK_HH_ */
