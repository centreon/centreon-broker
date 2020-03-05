/*
** Copyright 2019 Centreon
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
#ifndef CCB_SQL_CONFLICT_MANAGER_HH
#define CCB_SQL_CONFLICT_MANAGER_HH
#include <array>
#include <condition_variable>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/misc/pair.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/storage/stored_timestamp.hh"

CCB_BEGIN()
/* Forward declarations */
namespace neb {
class service_status;
}

namespace sql {
class conflict_manager {
  /* Forward declarations */
 public:
  enum stream_type {
    sql,
    storage
  };

 private:
  enum actions {
    none = 0,
    acknowledgements = 1 << 1,
    comments = 1 << 2,
    custom_variables = 1 << 3,
    downtimes = 1 << 4,
    host_dependencies = 1 << 5,
    host_hostgroups = 1 << 6,
    host_parents = 1 << 7,
    hostgroups = 1 << 8,
    hosts = 1 << 9,
    instances = 1 << 10,
    modules = 1 << 11,
    service_dependencies = 1 << 12,
    service_servicegroups = 1 << 13,
    servicegroups = 1 << 14,
    services = 1 << 15,
    index_data = 1 << 16,
    metrics = 1 << 17,
  };

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

  static int32_t (conflict_manager::*const _neb_processing_table[])();
  static conflict_manager* _singleton;
  static std::mutex _init_m;
  static std::condition_variable _init_cv;

  /* When events arrive in the conflict_manager, two things are done.
   * A boolean is inserted at the end of the timeline with the value false.
   * A tuple is made with the event, the stream_type and a pointer to the
   * boolean.
   * This tuple is inserted at the end of the queue.
   * The idea behind all of this is we can treat events by order. When an event
   * is done, we have access to the boolean stored in the timeline to set it to
   * true.
   * And later, the stream that sent events will know how many events can be
   * released from the retention queue. */
  std::deque<std::tuple<std::shared_ptr<io::data>, stream_type, bool*> >
      _events;

  /* Since the sql and storage streams use this conflict_manager, we must
   * manage two queues, the first one for sql and the second one for storage.
   * So they will know when their events will be released. */
  std::array<std::deque<bool>, 2> _timeline;

  /* This array stores how many events for each connector have been
   * acknowledged. */
  std::array<int32_t, 2> _ack;

  /* Current actions by connection */
  std::vector<uint32_t> _action;

  mutable std::mutex _loop_m;
  std::condition_variable _loop_cv;
  bool _exit;
  uint32_t _loop_timeout;
  uint32_t _max_pending_queries;
  uint32_t _pending_queries;
  mysql _mysql;
  uint32_t _instance_timeout;
  bool _store_in_db;
  uint32_t _rrd_len;
  uint32_t _interval_length;

  std::thread _thread;

  /* Stats */
  std::mutex _stat_m;
  int32_t _still_pending_events;
  int32_t _loop_duration;
  int32_t _speed;

  std::unordered_set<uint32_t> _cache_deleted_instance_id;
  std::unordered_map<uint32_t, uint32_t> _cache_host_instance;
  std::unordered_map<uint64_t, size_t> _cache_hst_cmd;
  std::unordered_map<std::pair<uint64_t, uint64_t>, size_t> _cache_svc_cmd;
  std::unordered_map<std::pair<uint64_t, uint64_t>, index_info> _index_cache;
  std::unordered_map<std::pair<uint32_t, std::string>, metric_info>
      _metric_cache;
  std::unordered_set<uint32_t> _hostgroup_cache;
  std::unordered_set<uint32_t> _servicegroup_cache;
  std::deque<metric_value> _perfdata_queue;
  timestamp _oldest_timestamp;
  std::unordered_map<uint32_t, stored_timestamp> _stored_timestamps;

  database::mysql_stmt _acknowledgement_insupdate;
  database::mysql_stmt _comment_insupdate;
  database::mysql_stmt _custom_variable_delete;
  database::mysql_stmt _custom_variable_insupdate;
  database::mysql_stmt _custom_variable_status_insupdate;
  database::mysql_stmt _downtime_insupdate;
  database::mysql_stmt _event_handler_insupdate;
  database::mysql_stmt _flapping_status_insupdate;
  database::mysql_stmt _host_check_update;
  database::mysql_stmt _host_dependency_insupdate;
  database::mysql_stmt _host_group_insupdate;
  database::mysql_stmt _host_group_member_delete;
  database::mysql_stmt _host_group_member_insert;
  database::mysql_stmt _host_insupdate;
  database::mysql_stmt _host_parent_delete;
  database::mysql_stmt _host_parent_insert;
  database::mysql_stmt _host_status_update;
  database::mysql_stmt _instance_insupdate;
  database::mysql_stmt _instance_status_insupdate;
  database::mysql_stmt _log_insert;
  database::mysql_stmt _module_insert;
  database::mysql_stmt _service_check_update;
  database::mysql_stmt _service_dependency_insupdate;
  database::mysql_stmt _service_group_insupdate;
  database::mysql_stmt _service_group_member_delete;
  database::mysql_stmt _service_group_member_insert;
  database::mysql_stmt _service_insupdate;
  database::mysql_stmt _service_status_update;

  database::mysql_stmt _index_data_insert;
  database::mysql_stmt _index_data_update;
  database::mysql_stmt _index_data_query;
  database::mysql_stmt _metrics_insert;
  database::mysql_stmt _metrics_update;

  conflict_manager(database_config const& dbcfg,
                   uint32_t loop_timeout,
                   uint32_t instance_timeout);
  conflict_manager() = delete;
  conflict_manager& operator=(conflict_manager const& other) = delete;
  conflict_manager(conflict_manager const& other) = delete;
  ~conflict_manager();
  bool _should_exit() const;
  void _callback();

  void _update_hosts_and_services_of_unresponsive_instances();
  void _update_hosts_and_services_of_instance(uint32_t id, bool responsive);
  void _update_timestamp(uint32_t instance_id);
  bool _is_valid_poller(uint32_t instance_id);
  void _check_deleted_index();

  int32_t _process_acknowledgement();
  int32_t _process_comment();
  int32_t _process_custom_variable();
  int32_t _process_custom_variable_status();
  int32_t _process_downtime();
  int32_t _process_event_handler();
  int32_t _process_flapping_status();
  int32_t _process_host_check();
  int32_t _process_host_dependency();
  int32_t _process_host_group();
  int32_t _process_host_group_member();
  int32_t _process_host();
  int32_t _process_host_parent();
  int32_t _process_host_status();
  int32_t _process_instance();
  int32_t _process_instance_status();
  int32_t _process_log();
  int32_t _process_module();
  int32_t _process_service_check();
  int32_t _process_service_dependency();
  int32_t _process_service_group();
  int32_t _process_service_group_member();
  int32_t _process_service();
  int32_t _process_service_status();
  int32_t _process_instance_configuration();
  int32_t _process_responsive_instance();

  int32_t _storage_process_service_status();

  void _load_deleted_instances();
  void _load_caches();
  void _clean_tables(uint32_t instance_id);
  void _prepare_hg_insupdate_statement();
  void _prepare_sg_insupdate_statement();
  void _finish_action(int32_t conn, uint32_t action);
  void _finish_actions();
  void _add_action(int32_t conn, actions action);
  void _insert_perfdatas();
  void _pop_event(std::tuple<std::shared_ptr<io::data>, stream_type, bool*>& p);
  std::size_t inline _get_events_size() const {
    std::lock_guard<std::mutex> lk(_loop_m);
    return _events.size();
  }

 public:
  static void init_sql(database_config const& dbcfg,
                       uint32_t loop_timeout,
                       uint32_t instance_timeout);
  static bool init_storage(bool store_in_db,
                           uint32_t rrd_len,
                           uint32_t interval_length);
  static void close();
  static conflict_manager& instance();
  void exit();
  static void unload();
  json11::Json::object get_statistics();

  void send_event(stream_type c, std::shared_ptr<io::data> const& e);
  int32_t get_acks(stream_type c);
};
}  // namespace sql
CCB_END()

#endif /* !CCB_SQL_CONFLICT_MANAGER_HH */
