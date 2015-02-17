/*
** Copyright 2011-2012,2015 Merethis
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

#ifndef CCB_LOCAL_CONNECTOR_HH
#  define CCB_LOCAL_CONNECTOR_HH

#  include <QLocalSocket>
#  include <QMutex>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                          local {
  /**
   *  @class connector connector.hh "com/centreon/broker/local/connector.hh"
   *  @brief Local socket connector.
   *
   *  Connect to some local socket.
   */
  class                            connector : public io::endpoint {
  public:
                                   connector();
                                   connector(connector const& c);
                                   ~connector();
    connector&                     operator=(connector const& c);
    io::endpoint*                  clone() const;
    void                           close();
    void                           connect_to(QString const& name);
    misc::shared_ptr<io::stream>   open();
    misc::shared_ptr<io::stream>   open(QString const& id);
    void                           set_timeout(int msecs);

  private:
    void                           _internal_copy(connector const& c);

    misc::shared_ptr<QMutex>       _mutex;
    QString                        _name;
    misc::shared_ptr<QLocalSocket> _socket;
    int                            _timeout;
  };
}

CCB_END()

#endif // !CCB_LOCAL_CONNECTOR_HH
