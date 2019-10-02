/*
** Copyright 2011-2017 Centreon
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

#ifndef CCB_STORAGE_STREAM_HH
#define CCB_STORAGE_STREAM_HH

#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/namespace.hh"
//#include "com/centreon/broker/storage/rebuilder.hh"

CCB_BEGIN()

// Forward declaration.
class database_config;

namespace storage {
/**
 *  @class stream stream.hh "com/centreon/broker/storage/stream.hh"
 *  @brief Storage stream.
 *
 *  Handle perfdata and insert proper informations in index_data and
 *  metrics table of a centstorage DB.
 */
class stream : public io::stream {
 public:
  stream(uint32_t rrd_len,
         uint32_t interval_length,
         uint32_t rebuild_check_interval,
         bool store_in_db = true);
  ~stream();
  int32_t flush();
  bool read(std::shared_ptr<io::data>& d, time_t deadline);
  void statistics(json11::Json::object& tree) const;
  void update();
  int32_t write(std::shared_ptr<io::data> const& d);
  void ack_pending_events(int32_t v);

 private:
  struct index_info {
    std::string host_name;
    uint32_t index_id;
    bool locked;
    uint32_t rrd_retention;
    std::string service_description;
    bool special;
  };
  struct metric_info {
    bool locked;
    uint32_t metric_id;
    uint32_t type;
    double value;
    std::string unit_name;
    double warn;
    double warn_low;
    bool warn_mode;
    double crit;
    double crit_low;
    bool crit_mode;
    double min;
    double max;
  };
  struct metric_value {
    time_t c_time;
    uint32_t metric_id;
    short status;
    double value;
  };

  stream(stream const& other);
  stream& operator=(stream const& other);
  void _check_deleted_index();
  void _delete_metrics(std::list<unsigned long long> const& metrics_to_delete);
  uint32_t _find_index_id(uint64_t host_id,
                              uint64_t service_id,
                              std::string const& host_name,
                              std::string const& service_desc,
                              uint32_t* rrd_len,
                              bool* locked);
  uint64_t _find_metric_id(uint64_t index_id,
                           std::string metric_name,
                           std::string const& unit_name,
                           double warn,
                           double warn_low,
                           bool warn_mode,
                           double crit,
                           double crit_low,
                           bool crit_mode,
                           double min,
                           double max,
                           double value,
                           uint32_t* type,
                           bool* locked);
  void _host_instance_cache_create();
  void _insert_perfdatas_new();
  void _insert_perfdatas();
  void _process_host(std::shared_ptr<io::data> const& e);
  void _process_instance(std::shared_ptr<io::data> const& e);
  void _rebuild_cache();
  void _update_status(std::string const& status);
  void _set_ack_events();

  std::map<std::pair<uint64_t, uint64_t>, index_info> _index_cache;
  uint32_t _interval_length;
  int32_t _ack_events;
  int32_t _pending_events;
  std::map<std::pair<uint64_t, std::string>, metric_info> _metric_cache;
  std::deque<metric_value> _perfdata_queue;
  std::map<uint32_t, uint32_t> _cache_host_instance;
  //rebuilder _rebuilder;
  uint32_t _rrd_len;
  std::string _status;
  mutable std::mutex _statusm;
  bool _store_in_db;
};
}  // namespace storage

CCB_END()

#endif  // !CCB_STORAGE_STREAM_HH
