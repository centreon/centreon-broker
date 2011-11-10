/*
** Copyright 2011 Merethis
**
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

# include <QMap>
# include <QMutex>
# include <QObject>
# include <QScopedPointer>
# include <QSharedPointer>
# include <QTcpServer>
# include <QTcpSocket>
# include "com/centreon/broker/io/endpoint.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          tcp {
        /**
         *  @class acceptor acceptor.hh "com/centreon/broker/tcp/acceptor.hh"
         *  @brief TCP acceptor.
         *
         *  Accept TCP connections.
         */
        class            acceptor : public QObject, public io::endpoint {
          Q_OBJECT

         private:
          QString        _ca;
          QMap<QTcpSocket*, QSharedPointer<QMutex> >
                         _children;
          QMutex         _childrenm;
          QMutex         _mutex;
          unsigned short _port;
          QString        _private;
          QString        _public;
          QScopedPointer<QTcpServer>
                         _socket;
          bool           _tls;
          void           _internal_copy(acceptor const& a);

         private slots:
          void           _on_stream_destroy();

         public:
                         acceptor();
                         acceptor(acceptor const& a);
                         ~acceptor();
          acceptor&      operator=(acceptor const& a);
          void           close();
          void           listen_on(unsigned short port);
          QSharedPointer<io::stream>
                         open();
          void           set_tls(
                           bool enable,
                           QString const& private_key = QString(),
                           QString const& public_cert = QString(),
                           QString const& ca_cert = QString());
        };
      }
    }
  }
}

#endif /* !CCB_TCP_ACCEPTOR_HH_ */
