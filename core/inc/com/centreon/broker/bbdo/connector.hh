/*
** Copyright 2013,2015 Merethis
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

#  include <ctime>
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
                  connector(
                    bool negociate,
                    QString const& extensions,
                    time_t timeout,
                    bool coarse = false);
                  connector(connector const& other);
                  ~connector();
    connector&    operator=(connector const& other);
    void          close();
    misc::shared_ptr<io::stream>
                  open();

  private:
    misc::shared_ptr<io::stream>
                  _open(misc::shared_ptr<io::stream> stream);

    bool          _coarse;
    QString       _extensions;
    bool          _negociate;
    time_t        _timeout;
  };
}

CCB_END()

#endif // !CCB_BBDO_CONNECTOR_HH
