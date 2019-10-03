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

CCB_BEGIN()

namespace storage {
/**
 *  @class rebuilder rebuilder.hh "com/centreon/broker/storage/rebuilder.hh"
 *  @brief Check for graphs to be rebuild.
 *
 *  Check for graphs to be rebuild at fixed interval.
 */
class rebuilder {
 public:
  rebuilder(database_config const& db_cfg,
            uint32_t rebuild_check_interval = 600,
            uint32_t rrd_length = 15552000,
            uint32_t interval_length = 60);
  ~rebuilder();
  uint32_t get_rebuild_check_interval() const throw();
  uint32_t get_rrd_length() const throw();

 private:
  // Local types.
  struct index_info {
    uint32_t index_id;
    uint32_t host_id;
    uint32_t service_id;
    uint32_t rrd_retention;
  };

  struct metric_info {
    uint32_t metric_id;
    std::string metric_name;
    short metric_type;
  };

  rebuilder(rebuilder const& other);
  rebuilder& operator=(rebuilder const& other);
  void _next_index_to_rebuild(index_info& info, mysql& ms);
  void _rebuild_metric(mysql& ms,
                       uint32_t metric_id,
                       uint32_t host_id,
                       uint32_t service_id,
                       std::string const& metric_name,
                       short metric_type,
                       uint32_t interval,
                       unsigned length);
  void _rebuild_status(mysql& ms, uint32_t index_id, uint32_t interval);
  void _send_rebuild_event(bool end, uint32_t id, bool is_index);
  void _set_index_rebuild(mysql& db, uint32_t index_id, short state);
  void _run();

  std::unique_ptr<std::thread> _thread;
  database_config _db_cfg;
  std::shared_ptr<mysql_connection> _connection;
  uint32_t _interval_length;
  uint32_t _rebuild_check_interval;
  uint32_t _rrd_len;
  std::condition_variable _cond_should_exit;
  std::mutex _mutex_should_exit;
  volatile bool _should_exit;
};
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_REBUILDER_HH
