/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
    stream*        _child;
  };
}

CCB_END()

#endif // !CCB_TCP_CONNECTOR_HH
