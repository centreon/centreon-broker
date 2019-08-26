/*
** Copyright 2012-2015,2017 Centreon
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

#ifndef CCB_STORAGE_REBUILDER_HH
#  define CCB_STORAGE_REBUILDER_HH

#  include <memory>
#  include <QThread>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
class               database;

namespace           storage {
  /**
   *  @class rebuilder rebuilder.hh "com/centreon/broker/storage/rebuilder.hh"
   *  @brief Check for graphs to be rebuild.
   *
   *  Check for graphs to be rebuild at fixed interval.
   */
  class             rebuilder : public QThread {
   public:
                    rebuilder(
                      database_config const& db_cfg,
                      unsigned int rebuild_check_interval = 600,
                      unsigned int rrd_length = 15552000,
                      unsigned int interval_length = 60);
                    ~rebuilder() throw ();
    void            exit() throw ();
    unsigned int    get_rebuild_check_interval() const throw ();
    unsigned int    get_rrd_length() const throw ();
    void            run();

   private:
    // Local types.
    struct index_info {
      unsigned int index_id;
      unsigned int host_id;
      unsigned int service_id;
      unsigned int rrd_retention;
    };

    struct metric_info {
      unsigned int metric_id;
      std::string metric_name;
      short metric_type;
    };

                    rebuilder(rebuilder const& other);
    rebuilder&      operator=(rebuilder const& other);
    void            _next_index_to_rebuild(
                      index_info& info,
                      database& db);
    void            _rebuild_metric(
                      database& db,
                      unsigned int metric_id,
                      unsigned int host_id,
                      unsigned int service_id,
                      std::string const& metric_name,
                      short metric_type,
                      unsigned int interval,
                      unsigned length);
    void            _rebuild_status(
                      database& db,
                      unsigned int index_id,
                      unsigned int interval);
    void            _send_rebuild_event(
                      bool end,
                      unsigned int id,
                      bool is_index);
    void            _set_index_rebuild(
                      database& db,
                      unsigned int index_id,
                      short state);

    database_config _db_cfg;
    unsigned int    _interval_length;
    unsigned int    _rebuild_check_interval;
    unsigned int    _rrd_len;
    volatile bool   _should_exit;
  };
}

CCB_END()

#endif // !CCB_STORAGE_REBUILDER_HH
