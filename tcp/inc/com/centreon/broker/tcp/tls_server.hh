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

#ifndef CCB_TCP_TLS_SERVER_HH_
# define CCB_TCP_TLS_SERVER_HH_

# include <QString>
# include <QTcpServer>
# if QT_VERSION < 0x040700
#  include <QQueue>
# endif /* QT_VERSION < 4.7 */

namespace             com {
  namespace           centreon {
    namespace         broker {
      namespace       tcp {
        /**
         *  @class tls_server tls_server.hh "com/centreon/broker/tcp/tls_server.hh"
         *  @brief TLS server.
         *
         *  Accept incoming TLS connections.
         */
        class         tls_server : public QTcpServer {
          Q_OBJECT

         private:
          QString     _ca;
          QString     _private;
          QString     _public;
                      tls_server(tls_server const& ts);
          tls_server& operator=(tls_server const& ts);

         public:
                      tls_server(QString const& private_key = QString(),
                        QString const& public_cert = QString(),
                        QString const& ca_cert = QString());
                      ~tls_server();
          void        incomingConnection(int socketDescriptor);

#if QT_VERSION < 0x040700
         private:
          QQueue<QTcpSocket*>
                      _pending;

         public:
          bool        hasPendingConnections() const;
          QTcpSocket* nextPendingConnection();
#endif /* QT_VERSION < 4.7 */
        };
      }
    }
  }
}

#endif /* !CCB_TCP_TLS_SERVER_HH_ */
