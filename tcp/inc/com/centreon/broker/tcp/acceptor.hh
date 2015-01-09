/*
** Copyright 2011-2013,2015 Merethis
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

#ifndef CCB_TCP_ACCEPTOR_HH
#  define CCB_TCP_ACCEPTOR_HH

#  include <memory>
#  include <QMap>
#  include <QMutex>
#  include <QObject>
#  include <QPair>
#  include <QTcpServer>
#  include <QTcpSocket>
#  include "com/centreon/broker/io/endpoint.hh"

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

         public:
                         acceptor();
                         acceptor(acceptor const& a);
                         ~acceptor();
          acceptor&      operator=(acceptor const& a);
          io::endpoint*  clone() const;
          void           close();
          void           listen_on(unsigned short port);
          misc::shared_ptr<io::stream>
                         open(persistent_cache* cache = NULL);
          misc::shared_ptr<io::stream>
                         open(
                           QString const& id,
                           persistent_cache* cache = NULL);
          void           stats(io::properties& tree);

         private:
          void           _internal_copy(acceptor const& a);

          QList<QPair<misc::shared_ptr<QTcpSocket>, misc::shared_ptr<QMutex> > >
                         _children;
          QMutex         _childrenm;
          QMutex         _mutex;
          unsigned short _port;
          std::auto_ptr<QTcpServer>
                         _socket;

         private slots:
          void           _on_stream_destroy(QObject* obj);
          void           _on_stream_disconnected();
          void           _on_stream_error(
                           QAbstractSocket::SocketError e);
        };
      }
    }
  }
}

#endif // !CCB_TCP_ACCEPTOR_HH
