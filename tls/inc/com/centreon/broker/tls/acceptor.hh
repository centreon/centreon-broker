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

#ifndef CCB_TLS_ACCEPTOR_HH
#  define CCB_TLS_ACCEPTOR_HH

#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         tls {
  /**
   *  @class acceptor acceptor.hh "com/centreon/broker/tls/acceptor.hh"
   *  @brief Perform TLS verification on top of another acceptor.
   *
   *  Within the process of accepting an incoming client, the TLS
   *  acceptor class will provide encryption to the lower stream. Using
   *  this class is really simple : build the object, set some
   *  properties and call open(). Then use it just like you'd use
   *  another io::stream. Encryption will be automatically provided on
   *  the returned accepted streams.
   */
  class           acceptor : public io::endpoint {
  public:
                  acceptor(
                    std::string const& cert = std::string(),
                    std::string const& key = std::string(),
                    std::string const& ca = std::string());
                  acceptor(acceptor const& right);
                  ~acceptor();
    acceptor&     operator=(acceptor const& right);
    misc::shared_ptr<io::stream>
                  open();
    misc::shared_ptr<io::stream>
                  open(misc::shared_ptr<io::stream> lower);

  private:
    void          _internal_copy(acceptor const& right);

    std::string   _ca;
    std::string   _cert;
    std::string   _key;
  };
}

CCB_END()

#endif // !CCB_TLS_ACCEPTOR_HH
