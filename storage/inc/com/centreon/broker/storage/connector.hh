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

#ifndef CCB_STORAGE_CONNECTOR_HH
#  define CCB_STORAGE_CONNECTOR_HH

#  include <ctime>
#  include <QString>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace           storage {
  /**
   *  @class connector connector.hh "com/centreon/broker/storage/connector.hh"
   *  @brief Connect to a database.
   *
   *  Send perfdata in a Centreon Storage database.
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
                      database_config const& db_cfg,
                      unsigned int rrd_len,
                      time_t interval_length,
                      unsigned int rebuild_check_interval,
                      bool store_in_data_bin = true,
                      bool insert_in_index_data = false);
    misc::shared_ptr<io::stream>
                    open();

   private:
    void            _internal_copy(connector const& other);

    database_config _db_cfg;
    bool            _insert_in_index_data;
    time_t          _interval_length;
    unsigned int    _rebuild_check_interval;
    unsigned int    _rrd_len;
    bool            _store_in_data_bin;
  };
}

CCB_END()

#endif // !CCB_STORAGE_CONNECTOR_HH
