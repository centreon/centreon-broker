/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_INFLUXDB_CONNECTOR_HH
#  define CCB_INFLUXDB_CONNECTOR_HH

#  include <ctime>
#  include <QString>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           influxdb {
  /**
   *  @class connector connector.hh "com/centreon/broker/influxdb/connector.hh"
   *  @brief Connect to an influxdb stream.
   */
  class             connector : public io::endpoint {
  public:
                    connector();
                    connector(connector const& other);
                    ~connector();
    connector&      operator=(connector const& other);
    io::endpoint*   clone() const;
    void            close();
    void            connect_to(
                      std::string const& user,
                      std::string const& passwd,
                      std::string const& addr,
                      unsigned short _port,
                      std::string const& db,
                      unsigned int queries_per_transaction);
    misc::shared_ptr<io::stream>
                    open();
    misc::shared_ptr<io::stream>
                    open(QString const& id);

   private:
    std::string     _user;
    std::string     _password;
    std::string     _addr;
    unsigned short  _port;
    std::string     _db;
    unsigned int    _queries_per_transaction;

    void            _internal_copy(connector const& other);
  };
}

CCB_END()

#endif // !CCB_INFLUXDB_CONNECTOR_HH
