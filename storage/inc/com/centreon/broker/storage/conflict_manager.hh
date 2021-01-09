/*
** Copyright 2019-2021 Centreon
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
#include <atomic>
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
#include "com/centreon/broker/misc/mfifo.hh"
#include "com/centreon/broker/misc/pair.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/storage/perfdata.hh"
#include "com/centreon/broker/storage/stored_timestamp.hh"

CCB_BEGIN()
/* Forward declarations */
namespace neb {
class service_status;
}

namespace storage {

/**
 * @brief The conflict manager.
 *
 * Many queries are executed by Broker through the sql connector and also
 * the storage connector. Thos queries are made with several connections to
 * the database and we don't commit after each query. All those constraints
 * are there for performance purpose but they can lead us to database
 * deadlocks. To avoid such locks, there is the conflict manager. Sent queries
 * are sent to connections through it. The idea behind the conflict manager
 * is the following:
 *
 * * determine the connection to use for the upcoming query.
 * * Check that the "action" to execute is compatible with actions already
 *   running on our connection and also others. If not, solve the issue with
 *   commits.
 * * Send the query to the connection.
 * * Add or not an action flag to this connection for next queries.
 *
 * Another task of the conflict manager is to keep informations for queries.
 * Metrics, customvariables are sent in bulk to avoid locks on the database,
 * so we keep some containers here to build those big queries.
 *
 * The conflict manager works with two streams: sql and storage.
 *
 * To initialize it, two functions are used:
 * * init_sql(): initialization of the sql part.
 * * init_storage(): initialization of the storage part. This one needs the
 *   sql part to be initialized before. If it is not already initialized, this
 *   function waits for it (with a timeout).
 *
 * Once the object is initialized, we have the classical static internal method
 * `instance()`.
 */
class conflict_manager {
  /* Forward declarations */
 public:
  enum instance_state { not_started, running, finished };
  enum stream_type { sql, storage };

 private:
  enum actions {
    none = 0,
    acknowledgements = 1 << 0,
    comments = 1 << 1,
    custom_variables = 1 << 2,
    downtimes = 1 << 3,
    host_dependencies = 1 << 4,
    host_hostgroups = 1 << 5,
    host_parents = 1 << 6,
    hostgroups = 1 << 7,
    hosts = 1 << 8,
    instances = 1 << 9,
    modules = 1 << 10,
    service_dependencies = 1 << 11,
    service_servicegroups = 1 << 12,
    servicegroups = 1 << 13,
    services = 1 << 14,
    index_data = 1 << 15,
    metrics = 1 << 16,
  };

  struct index_info {
    std::string host_name;
    uint64_t index_id;
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

  static void (conflict_manager::*const _neb_processing_table[])(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>&);
  static conflict_manager* _singleton;
  static instance_state _state;
  static std::mutex _init_m;
  static std::condition_variable _init_cv;

  misc::mfifo<std::shared_ptr<io::data>, 2> _fifo;

  /* Current actions by connection */
  std::vector<uint32_t> _action;

  mutable std::mutex _loop_m;
  std::condition_variable _loop_cv;
  bool _exit;
  std::atomic_bool _broken;
  uint32_t _loop_timeout;
  int32_t _max_pending_queries;
  mysql _mysql;
  uint32_t _instance_timeout;
  bool _store_in_db;
  uint32_t _rrd_len;
  uint32_t _interval_length;
  uint32_t _max_perfdata_queries;
  uint32_t _max_metrics_queries;
  uint32_t _max_cv_queries;
  uint32_t _max_log_queries;

  std::thread _thread;

  /* Stats */
  std::mutex _stat_m;
  int32_t _events_handled;
  float _speed;
  std::array<float, 20> _stats_count;
  int32_t _stats_count_pos;

  /* How many streams are using this conflict_manager? */
  std::atomic<uint32_t> _ref_count;

  std::unordered_set<uint32_t> _cache_deleted_instance_id;
  std::unordered_map<uint32_t, uint32_t> _cache_host_instance;
  std::unordered_map<uint64_t, size_t> _cache_hst_cmd;
  std::unordered_map<std::pair<uint64_t, uint64_t>, size_t> _cache_svc_cmd;
  std::unordered_map<std::pair<uint64_t, uint64_t>, index_info> _index_cache;
  std::unordered_map<std::pair<uint64_t, std::string>, metric_info>
      _metric_cache;
  std::mutex _metric_cache_m;

  std::unordered_set<uint32_t> _hostgroup_cache;
  std::unordered_set<uint32_t> _servicegroup_cache;

  /* The queue of metrics sent in bulk to the database. The insert is done if
   * the loop timeout is reached or if the queue size is greater than
   * _max_perfdata_queries. The filled table here is 'data_bin'. */
  std::deque<metric_value> _perfdata_queue;
  /* This map is also sent in bulk to the database. The insert is done if
   * the loop timeout is reached or if the queue size is greater than
   * _max_metrics_queries. Values here are the real time values, so if the
   * same metric is recevied two times, the new value can overwrite the old
   * one, that's why we store those values in a map. The filled table here is
   * 'metrics'. */
  std::unordered_map<int32_t, metric_info*> _metrics;

  /* These queues are sent in bulk to the database. The insert/update is done
   * if the loop timeout is reached or if the queue size is greater than
   * _max_cv_queries/_max_log_queries. The filled table here is respectively
   * 'customvariables'/'logs'. The queue elements are pairs of a string used
   * for the query and a pointer to a boolean so that we can acknowledge the
   * BBDO event when written. */
  std::deque<std::pair<bool*, std::string>> _cv_queue;
  std::deque<std::pair<bool*, std::string>> _log_queue;

  timestamp _oldest_timestamp;
  std::unordered_map<uint32_t, stored_timestamp> _stored_timestamps;

  database::mysql_stmt _acknowledgement_insupdate;
  database::mysql_stmt _comment_insupdate;
  database::mysql_stmt _custom_variable_delete;
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

  void _process_acknowledgement(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_comment(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_custom_variable(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_custom_variable_status(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_downtime(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_event_handler(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_flapping_status(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host_check(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host_dependency(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host_group(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host_group_member(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host(std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host_parent(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_host_status(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_instance(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_instance_status(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_log(std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_module(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_service_check(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_service_dependency(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_service_group(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_service_group_member(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_service(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_service_status(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_instance_configuration(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);
  void _process_responsive_instance(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);

  void _storage_process_service_status(
      std::tuple<std::shared_ptr<io::data>, uint32_t, bool*>& t);

  void _load_deleted_instances();
  void _load_caches();
  void _clean_tables(uint32_t instance_id);
  void _prepare_hg_insupdate_statement();
  void _prepare_sg_insupdate_statement();
  void _finish_action(int32_t conn, uint32_t action);
  void _finish_actions();
  void _add_action(int32_t conn, actions action);
  void _update_metrics();
  void _insert_perfdatas();
  void _update_customvariables();
  void _insert_logs();
  void __exit();

 public:
  static bool init_sql(database_config const& dbcfg,
                       uint32_t loop_timeout,
                       uint32_t instance_timeout);
  static bool init_storage(bool store_in_db,
                           uint32_t rrd_len,
                           uint32_t interval_length,
                           uint32_t max_pending_queries);
  static conflict_manager& instance();
  int32_t unload(stream_type type);
  json11::Json::object get_statistics();

  int32_t send_event(stream_type c, std::shared_ptr<io::data> const& e);
  int32_t get_acks(stream_type c);
  void update_metric_info_cache(uint32_t index_id,
                                uint32_t metric_id,
                                std::string const& metric_name,
                                short metric_type);
};
}  // namespace storage
CCB_END()

#endif /* !CCB_SQL_CONFLICT_MANAGER_HH */
