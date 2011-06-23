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

#ifndef CCB_LOCAL_ACCEPTOR_HH_
# define CCB_LOCAL_ACCEPTOR_HH_

# include <QLocalServer>
# include <QScopedPointer>
# include <QString>
# include "com/centreon/broker/io/endpoint.hh"

namespace                              com {
  namespace                            centreon {
    namespace                          broker {
      namespace                        local {
        /**
         *  @class acceptor acceptor.hh "com/centreon/broker/local/acceptor.hh"
         *  @brief Local socket server.
         *
         *  Accept local connections.
         */
        class                          acceptor : public io::endpoint {
         private:
          QString                      _name;
          QScopedPointer<QLocalServer> _socket;

         public:
                                       acceptor();
                                       acceptor(acceptor const& a);
                                       ~acceptor();
          acceptor&                    operator=(acceptor const& a);
          void                         close();
          void                         listen_on(QString const& name);
          QSharedPointer<io::stream>   open();
        };
      }
    }
  }
}

#endif /* !CCB_LOCAL_ACCEPTOR_HH_ */
