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
#define CCB_STORAGE_REBUILDER_HH

#include <memory>

#include "com/centreon/broker/database_config.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/pool.hh"

CCB_BEGIN()

namespace storage {
/**
 *  @class rebuilder rebuilder.hh "com/centreon/broker/storage/rebuilder.hh"
 *  @brief Check for graphs to be rebuild.
 *
 *  Check for graphs to be rebuild at fixed interval.
 *
 *  We don't instantiate a thread to work on the rebuilder. Instead, we use
 *  the asio mechanism with a steady_timer. The main function is
 *  rebuilder::_run(). When the rebuilder is constructed, we instanciate _timer
 *  and ask to execute the _run function when it expires. When the _run()
 *  function finishes, it reschedules the timer to be executed a new time after
 *  _rebuild_check_interval seconds. The rebuild destructor cancels the timer.
 *
 *  Each execution of the timer is done using the thread pool accessible from
 *  the pool object. No new thread is created.
 */
class rebuilder {
  asio::steady_timer _timer;
  std::atomic_bool _should_exit;
  database_config _db_cfg;
  std::shared_ptr<mysql_connection> _connection;
  uint32_t _interval_length;
  uint32_t _rebuild_check_interval;
  uint32_t _rrd_len;

  // Local types.
  struct index_info {
    uint64_t index_id;
    uint32_t host_id;
    uint32_t service_id;
    uint32_t rrd_retention;
  };

  struct metric_info {
    uint32_t metric_id;
    std::string metric_name;
    short metric_type;
  };

  void _next_index_to_rebuild(index_info& info, mysql& ms);
  void _rebuild_metric(mysql& ms,
                       uint32_t metric_id,
                       uint32_t host_id,
                       uint32_t service_id,
                       std::string const& metric_name,
                       short metric_type,
                       uint32_t interval,
                       unsigned length);
  void _rebuild_status(mysql& ms,
                       uint64_t index_id,
                       uint32_t interval,
                       uint32_t length);
  void _send_rebuild_event(bool end, uint32_t id, bool is_index);
  void _set_index_rebuild(mysql& db, uint64_t index_id, short state);
  void _run(asio::error_code ec);

 public:
  rebuilder(database_config const& db_cfg,
            uint32_t rebuild_check_interval = 600,
            uint32_t rrd_length = 15552000,
            uint32_t interval_length = 60);
  ~rebuilder();
  rebuilder(rebuilder const& other) = delete;
  rebuilder& operator=(rebuilder const& other) = delete;
};
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_REBUILDER_HH
