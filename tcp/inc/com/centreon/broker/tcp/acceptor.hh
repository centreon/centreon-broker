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

# include <QMutex>
# include <QScopedPointer>
# include <QTcpServer>
# include "com/centreon/broker/io/endpoint.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          tcp {
  /**
   *  @class acceptor acceptor.hh "com/centreon/broker/tcp/acceptor.hh"
   *  @brief TCP acceptor.
   *
   *  Accept TCP connections.
   */
  class            acceptor : public io::endpoint {
   private:
    QString        _ca;
    QMutex         _mutex;
    unsigned short _port;
    QString        _private;
    QString        _public;
    QScopedPointer<QTcpServer>
                   _socket;
    bool           _tls;
    void           _internal_copy(acceptor const& a);

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

CCB_END()

#endif /* !CCB_TCP_ACCEPTOR_HH_ */
