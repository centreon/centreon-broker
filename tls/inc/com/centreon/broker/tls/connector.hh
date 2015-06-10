/*
** Copyright 2009-2013,2015 Merethis
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

#ifndef CCB_TLS_CONNECTOR_HH
#  define CCB_TLS_CONNECTOR_HH

#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         tls {
  /**
   *  @class connector connector.hh "com/centreon/broker/tls/connector.hh"
   *  @brief Connect to a TLS peer.
   *
   *  Use a connected TLS stream to connect to a TLS peer.
   */
  class           connector : public io::endpoint {
  public:
                  connector(
                    std::string const& cert = std::string(),
                    std::string const& key = std::string(),
                    std::string const& ca = std::string());
                  connector(connector const& right);
                  ~connector();
    connector&    operator=(connector const& right);
    void          close();
    misc::shared_ptr<io::stream>
                  open();
    misc::shared_ptr<io::stream>
                  open(misc::shared_ptr<io::stream> lower);

  private:
    void          _internal_copy(connector const& right);

    std::string   _ca;
    std::string   _cert;
    std::string   _key;
  };
}

CCB_END()

#endif // !CCB_TLS_CONNECTOR_HH
