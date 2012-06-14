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

#ifndef CCB_RRD_CONNECTOR_HH
#  define CCB_RRD_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          rrd {
  /**
   *  @class connector connector.hh "com/centreon/broker/rrd/connector.hh"
   *  @brief RRD connector.
   *
   *  Generate an RRD stream that will write files.
   */
  class            connector : public io::endpoint {
  public:
                   connector();
                   connector(connector const& c);
                   ~connector();
    connector&     operator=(connector const& c);
    void           close();
    misc::shared_ptr<io::stream>
                   open();
    void           set_cached_local(QString const& local_socket);
    void           set_cached_net(unsigned short port);
    void           set_metrics_path(QString const& metrics_path);
    void           set_status_path(QString const& status_path);

   private:
    QString        _cached_local;
    unsigned short _cached_port;
    QString        _metrics_path;
    QString        _status_path;
  };
}

CCB_END()

#endif // !CCB_RRD_CONNECTOR_HH
