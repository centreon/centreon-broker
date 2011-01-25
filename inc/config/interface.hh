/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef CONFIG_INTERFACE_HH_
# define CONFIG_INTERFACE_HH_

# include <memory>
# include <string>

namespace                    config {
  /**
   *  @class interface interface.hh "config/interface.hh"
   *  @brief Hold configuration of an interface.
   *
   *  An interface is an external source or destination for events. This can
   *  either be an XML stream, a database, a file, ... This class holds the
   *  configuration of an interface.
   */
  class                      interface {
   private:
    void                     _internal_copy(interface const& i);

   public:
    enum                     protocol_type {
      unknown_proto = 0,
      ndo,
      xml
    };
    enum                     type_t {
      unknown = 0,
      db2,
      file,
      ibase,
      ipv4_client,
      ipv4_server,
      ipv6_client,
      ipv6_server,
      mysql,
      odbc,
      oracle,
      postgresql,
      sqlite,
      tds,
      unix_client,
      unix_server
    };
    std::string              db;
    std::auto_ptr<interface> failover;
    std::string              failover_name;
    std::string              filename;
    std::string              host;
    std::string              name;
    std::string              net_iface;
    std::string              password;
    short                    port;
    protocol_type            protocol;
    std::string              socket;
    type_t                   type;
    std::string              user;
# ifdef USE_TLS
    std::string              ca;
    std::string              cert;
    bool                     compress;
    std::string              key;
    bool                     tls;
# endif /* USE_TLS */
                             interface();
                             interface(interface const& i);
                             ~interface();
    interface&               operator=(interface const& i);
    bool                     operator==(interface const& i) const;
    bool                     operator!=(interface const& i) const;
    bool                     operator<(interface const& i) const;
  };
}

#endif /* !CONFIG_INTERFACE_HH_ */
