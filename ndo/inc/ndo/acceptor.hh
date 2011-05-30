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

#ifndef CCB_NDO_ACCEPTOR_HH_
# define CCB_NDO_ACCEPTOR_HH_

# include "io/acceptor.hh"

namespace           com {
  namespace         centreon {
    namespace       broker {
      namespace     ndo {
        /**
         *  @class acceptor acceptor.hh "ndo/acceptor.hh"
         *  @brief NDO acceptor.
         *
         *  Accept incoming NDO connections.
         */
        class       acceptor : public com::centreon::broker::io::acceptor {
         private:
          bool      _is_out;

         public:
                    acceptor(bool is_out);
                    acceptor(acceptor const& a);
                    ~acceptor();
          acceptor& operator=(acceptor const& a);
          void      accept(QSharedPointer<com::centreon::broker::io::stream> ptr);
          void      close();
          void      open();
        };
      }
    }
  }
}

#endif /* !CCB_NDO_ACCEPTOR_HH_ */
