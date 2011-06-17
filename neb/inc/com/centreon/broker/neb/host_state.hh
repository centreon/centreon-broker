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

#ifndef CCB_NEB_HOST_STATE_HH_
# define CCB_NEB_HOST_STATE_HH_

# include "com/centreon/broker/neb/state.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          neb {
        /**
         *  @class host_state host_state.hh "com/centreon/broker/neb/host_state.hh"
         *  @brief Host state.
         *
         *  State of an host at a given time.
         */
        class            host_state : public state {
         public:
                         host_state();
                         host_state(host_state const& hs);
                         ~host_state();
          host_state&    operator=(host_state const& hs);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_HOST_STATE_HH_ */
