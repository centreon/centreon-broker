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

#ifndef CCB_TCP_CONNECTOR_HH
#  define CCB_TCP_CONNECTOR_HH

#  include <QString>
#  include <QTcpSocket>
#  include <QMutex>
#  include <QWaitCondition>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/tcp/socket_parent.hh"
#  include "com/centreon/broker/tcp/stream.hh"

CCB_BEGIN()

namespace          tcp {
  /**
   *  @class connector connector.hh "com/centreon/broker/tcp/connector.hh"
   *  @brief TCP connector.
   *
   *  Connect to some remote TCP host.
   */
  class            connector : public io::endpoint, public socket_parent {
  public:
                   connector();
                   connector(connector const& other);
                   ~connector();
    connector&     operator=(connector const& other);
    endpoint*      clone() const;
    void           close();
    void           connect_to(QString const& host, unsigned short port);
    misc::shared_ptr<io::stream>
                   open();
    misc::shared_ptr<io::stream>
                   open(QString const& id);
    void           set_read_timeout(int secs);
    void           set_write_timeout(int secs);
    void           add_child(stream& child);
    void           remove_child(stream& child);

  private:
    void           _internal_copy(connector const& other);

    QString        _host;
    unsigned short _port;
    int            _read_timeout;
    int            _write_timeout;
    QMutex         _mutex;
    QWaitCondition _child_closed_condvar;
    stream*        _child;
  };
}

CCB_END()

#endif // !CCB_TCP_CONNECTOR_HH
