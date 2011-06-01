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

#ifndef CCB_EVENTS_HOST_CHECK_HH_
# define CCB_EVENTS_HOST_CHECK_HH_

# include "events/check.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        events {
        /**
         *  @class host_check host_check.hh "events/host_check.hh"
         *  @brief Check that has been executed on a host.
         *
         *  Once a check has been executed on a host, an object of this class
         *  is sent.
         */
        class          host_check : public check {
         public:
                       host_check();
                       host_check(host_check const& hc);
          virtual      ~host_check();
          host_check&  operator=(host_check const& hc);
          unsigned int type() const;
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_HOST_CHECK_HH_ */
