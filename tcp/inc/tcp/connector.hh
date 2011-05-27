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

#ifndef CCB_TCP_CONNECTOR_HH_
# define CCB_TCP_CONNECTOR_HH_

# include <QSharedPointer>
# include <QString>
# include <QTcpSocket>
# include "io/connector.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          tcp {
        /**
         *  @class connector connector.hh "tcp/connector.hh"
         *  @brief TCP connector.
         *
         *  Connect to some remote TCP host.
         */
        class            connector : public com::centreon::broker::io::connector {
         private:
          QString        _host;
          unsigned short _port;
          QSharedPointer<QTcpSocket>
                         _socket;
          void           _internal_copy(connector const& c);

         public:
                         connector();
                         connector(connector const& c);
                         ~connector();
          connector&     operator=(connector const& c);
          void           close();
          void           connect(QSharedPointer<com::centreon::broker::io::stream> ptr);
          void           connect_to(QString const& host, unsigned short port);
          void           open();
        };
      }
    }
  }
}

#endif /* !CCB_TCP_CONNECTOR_HH_ */
