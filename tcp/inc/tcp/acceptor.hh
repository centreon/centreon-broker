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

#ifndef CCB_TCP_ACCEPTOR_HH_
# define CCB_TCP_ACCEPTOR_HH_

# include <QTcpServer>
# include "io/acceptor.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          tcp {
        /**
         *  @class acceptor acceptor.hh "tcp/acceptor.hh"
         *  @brief TCP acceptor.
         *
         *  Accept TCP connections.
         */
        class            acceptor : public com::centreon::broker::io::acceptor {
         private:
          unsigned short _port;
	  QTcpServer     _socket;
                         acceptor(acceptor const& a);
          acceptor&      operator=(acceptor const& a);

         public:
                         acceptor();
                         ~acceptor();
          void           accept(QSharedPointer<com::centreon::broker::io::stream> ptr);
          void           close();
          void           listen_on(unsigned short port);
          void           open();
        };
      }
    }
  }
}

#endif /* !CCB_TCP_ACCEPTOR_HH_ */
