/*
** Copyright 2013 Merethis
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

#ifndef CCB_BBDO_CONNECTOR_HH
#  define CCB_BBDO_CONNECTOR_HH

#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         bbdo {
  /**
   *  @class connector connector.hh "com/centreon/broker/bbdo/connector.hh"
   *  @brief BBDO connector.
   *
   *  Initiate direct BBDO protocol connections.
   */
  class           connector : public io::endpoint {
  public:
                  connector(bool is_in, bool is_out);
                  connector(connector const& right);
                  ~connector();
    connector&    operator=(connector const& right);
    io::endpoint* clone() const;
    void          close();
    misc::shared_ptr<io::stream>
                  open();

  private:
    bool          _is_in;
    bool          _is_out;
  };
}

CCB_END()

#endif // !CCB_BBDO_CONNECTOR_HH
