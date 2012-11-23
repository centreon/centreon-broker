/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_TCP_CONNECTOR_HH
#  define CCB_TCP_CONNECTOR_HH

#  include <QString>
#  include <QTcpSocket>
#  include <QMutex>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          tcp {
  /**
   *  @class connector connector.hh "com/centreon/broker/tcp/connector.hh"
   *  @brief TCP connector.
   *
   *  Connect to some remote TCP host.
   */
  class            connector : public io::endpoint {
  public:
                   connector();
                   connector(connector const& c);
                   ~connector();
    connector&     operator=(connector const& c);
    io::endpoint*  clone() const;
    void           close();
    void           connect_to(QString const& host, unsigned short port);
    misc::shared_ptr<io::stream>
                   open();
    void           set_timeout(int msecs);
    void           set_tls(
                     bool enable,
                     QString const& private_key = QString(),
                     QString const& public_cert = QString(),
                     QString const& ca_cert = QString());

  private:
    void           _internal_copy(connector const& c);

    QString        _ca;
    QString        _host;
    misc::shared_ptr<QMutex>
                   _mutex;
    unsigned short _port;
    QString        _private;
    QString        _public;
    misc::shared_ptr<QTcpSocket>
                   _socket;
    int            _timeout;
    bool           _tls;
  };
}

CCB_END()

#endif // !CCB_TCP_CONNECTOR_HH
