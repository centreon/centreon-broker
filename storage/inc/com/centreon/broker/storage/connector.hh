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

#ifndef CCB_STORAGE_CONNECTOR_HH
#  define CCB_STORAGE_CONNECTOR_HH

#  include <ctime>
#  include <QString>
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
    io::endpoint*  clone() const;
    void           close();
    void           connect_to(
                     QString const& storage_type,
                     QString const& storage_host,
                     unsigned short storage_port,
                     QString const& storage_user,
                     QString const& storage_password,
                     QString const& storage_db,
                     unsigned int queries_per_transaction,
                     unsigned int rrd_len,
                     time_t interval_length,
                     unsigned int rebuild_check_interval,
                     bool check_replication,
                     bool store_in_data_bin = true);
    misc::shared_ptr<io::stream>
                   open();
    misc::shared_ptr<io::stream>
                   open(QString const& id);

   private:
    void           _internal_copy(connector const& c);

    bool           _check_replication;
    time_t         _interval_length;
    unsigned int   _queries_per_transaction;
    unsigned int   _rebuild_check_interval;
    unsigned int   _rrd_len;
    QString        _storage_db;
    QString        _storage_host;
    QString        _storage_password;
    unsigned short _storage_port;
    QString        _storage_user;
    QString        _storage_type;
    bool           _store_in_data_bin;
  };
}

CCB_END()

#endif // !CCB_STORAGE_CONNECTOR_HH
