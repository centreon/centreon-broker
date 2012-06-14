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

#ifndef CCB_STORAGE_CONNECTOR_HH
#  define CCB_STORAGE_CONNECTOR_HH

#  include <QString>
#  include <time.h>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          storage {
  /**
   *  @class connector connector.hh "com/centreon/broker/storage/connector.hh"
   *  @brief Connect to a database.
   *
   *  Send perfdata in a Centreon Storage database.
   */
  class            connector : public io::endpoint {
  public:
                   connector();
                   connector(connector const& c);
                   ~connector();
    connector&     operator=(connector const& c);
    void           close();
    void           connect_to(
                     QString const& storage_type,
                     QString const& storage_host,
                     unsigned short storage_port,
                     QString const& storage_user,
                     QString const& storage_password,
                     QString const& storage_db,
                     unsigned int rrd_len,
                     time_t interval_length);
    misc::shared_ptr<io::stream>
                   open();

   private:
    void           _internal_copy(connector const& c);

    time_t         _interval_length;
    unsigned int   _rrd_len;
    QString        _storage_db;
    QString        _storage_host;
    QString        _storage_password;
    unsigned short _storage_port;
    QString        _storage_user;
    QString        _storage_type;
  };
}

CCB_END()

#endif // !CCB_STORAGE_CONNECTOR_HH
