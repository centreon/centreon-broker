/*
** Copyright 2015 Merethis
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

#ifndef CCB_CORRELATION_CONNECTOR_HH
#  define CCB_CORRELATION_CONNECTOR_HH

#  include <QString>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        correlation {
  /**
   *  @class connector connector.hh "com/centreon/broker/correlation/connector.hh"
   *  @brief Open a correlation stream.
   *
   *  Generate a correlation stream that will generation correlation
   *  events (issue, issue parenting, host/service state events, ...).
   */
  class                          connector : public io::endpoint {
  public:
                                 connector(
                                   QString const& correlation_file,
                                   bool passive = false,
                                   misc::shared_ptr<persistent_cache> cache
                                   = misc::shared_ptr<persistent_cache>());
                                 connector(connector const& other);
                                 ~connector();
    connector&                   operator=(connector const& other);
    void                         close();
    misc::shared_ptr<io::stream> open();

  private:
    misc::shared_ptr<persistent_cache>
                                 _cache;
    QString                      _correlation_file;
    bool                         _passive;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_CONNECTOR_HH
