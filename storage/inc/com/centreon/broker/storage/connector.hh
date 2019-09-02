/*
** Copyright 2011-2013,2017 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_STORAGE_CONNECTOR_HH
#  define CCB_STORAGE_CONNECTOR_HH

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
    void            connect_to(
                      database_config const& db_cfg,
                      unsigned int rrd_len,
                      unsigned int interval_length,
                      unsigned int rebuild_check_interval,
                      bool store_in_data_bin = true,
                      bool insert_in_index_data = false);
    std::shared_ptr<io::stream>
                    open();

   private:
    void            _internal_copy(connector const& other);

    database_config _db_cfg;
    bool            _insert_in_index_data;
    unsigned int    _interval_length;
    unsigned int    _rebuild_check_interval;
    unsigned int    _rrd_len;
    bool            _store_in_data_bin;
  };
}

CCB_END()

#endif // !CCB_STORAGE_CONNECTOR_HH
