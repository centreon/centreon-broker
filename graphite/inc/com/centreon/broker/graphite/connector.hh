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

#ifndef CCB_GRAPHITE_CONNECTOR_HH
#  define CCB_GRAPHITE_CONNECTOR_HH

#  include <ctime>
#  include <QString>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           graphite {
  /**
   *  @class connector connector.hh "com/centreon/broker/graphite/connector.hh"
   *  @brief Connect to a graphite stream.
   */
  class             connector : public io::endpoint {
  public:
                    connector();
                    connector(connector const& other);
                    ~connector();
    connector&      operator=(connector const& other);
    io::endpoint*   clone() const;
    void            close();
    void            connect_to(std::string const& metric_naming,
                      std::string const& status_naming,
                      std::string const& db_user,
                      std::string const& db_passwd,
                      std::string const& db_host,
                      unsigned short db_port,
                      unsigned int queries_per_transaction,
                      misc::shared_ptr<persistent_cache> const& cache);
    misc::shared_ptr<io::stream>
                    open();

   private:
    std::string     _metric_naming;
    std::string     _status_naming;
    std::string     _user;
    std::string     _password;
    std::string     _addr;
    unsigned short  _port;
    unsigned int    _queries_per_transaction;
    misc::shared_ptr<persistent_cache>
                    _persistent_cache;

    void            _internal_copy(connector const& other);
  };
}

CCB_END()

#endif // !CCB_GRAPHITE_CONNECTOR_HH
