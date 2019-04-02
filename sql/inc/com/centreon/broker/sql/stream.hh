/*
** Copyright 2011-2016 Centreon
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

#ifndef CCB_SQL_STREAM_HH
#  define CCB_SQL_STREAM_HH

#  include <map>
#  include <set>
#  include <string>
#  include "com/centreon/broker/mysql.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/sql/cleanup.hh"
#  include "com/centreon/broker/sql/stored_timestamp.hh"

CCB_BEGIN()

namespace          sql {
  /**
   *  @class stream stream.hh "com/centreon/broker/sql/stream.hh"
   *  @brief SQL stream.
   *
   *  Stream events into SQL database.
   */
  class                         stream : public io::stream {
  public:
                                stream(
                                  database_config const& dbcfg,
                                  unsigned int cleanup_check_interval,
                                  unsigned int instance_timeout,
                                  bool with_state_events,
                                  bool enable_cmd_cache);
                                ~stream();
    int                         flush();
    bool                        read(misc::shared_ptr<io::data>& d, time_t deadline);
    void                        update();
    int                         write(misc::shared_ptr<io::data> const& d);

  private:
                                stream(stream const& other);
    stream&                     operator=(stream const& other);
    void                        _cache_clean();
    void                        _cache_create();
    void                        _prepare_hg_insupdate_statement();
    void                        _prepare_sg_insupdate_statement();
    void                        _clean_empty_host_groups();
    void                        _clean_empty_service_groups();
    void                        _clean_tables(unsigned int instance_id);
    void                        _host_instance_cache_create();
    bool                        _is_valid_poller(unsigned int poller_id);
    void                        _process_acknowledgement(misc::shared_ptr<io::data> const& e);
    void                        _process_comment(misc::shared_ptr<io::data> const& e);
    void                        _process_custom_variable(misc::shared_ptr<io::data> const& e);
    void                        _process_custom_variable_status(misc::shared_ptr<io::data> const& e);
    void                        _process_downtime(misc::shared_ptr<io::data> const& e);
    void                        _process_engine(misc::shared_ptr<io::data> const& e);
    void                        _process_event_handler(misc::shared_ptr<io::data> const& e);
    void                        _process_flapping_status(misc::shared_ptr<io::data> const& e);
    void                        _process_host(misc::shared_ptr<io::data> const& e);
    void                        _process_host_check(misc::shared_ptr<io::data> const& e);
    void                        _process_host_dependency(misc::shared_ptr<io::data> const& e);
    void                        _process_host_group(misc::shared_ptr<io::data> const& e);
    void                        _process_host_group_member(misc::shared_ptr<io::data> const& e);
    void                        _process_host_parent(misc::shared_ptr<io::data> const& e);
    void                        _process_host_state(misc::shared_ptr<io::data> const& e);
    void                        _process_host_status(misc::shared_ptr<io::data> const& e);
    void                        _process_instance(misc::shared_ptr<io::data> const& e);
    void                        _process_instance_configuration(misc::shared_ptr<io::data> const& e);
    void                        _process_instance_status(misc::shared_ptr<io::data> const& e);
    void                        _process_issue(misc::shared_ptr<io::data> const& e);
    void                        _process_issue_parent(misc::shared_ptr<io::data> const& e);
    void                        _process_log(misc::shared_ptr<io::data> const& e);
    void                        _process_module(misc::shared_ptr<io::data> const& e);
    void                        _process_notification(misc::shared_ptr<io::data> const& e);
    void                        _process_service(misc::shared_ptr<io::data> const& e);
    void                        _process_service_check(misc::shared_ptr<io::data> const& e);
    void                        _process_service_dependency(misc::shared_ptr<io::data> const& e);
    void                        _process_service_group(misc::shared_ptr<io::data> const& e);
    void                        _process_service_group_member(misc::shared_ptr<io::data> const& e);
    void                        _process_service_state(misc::shared_ptr<io::data> const& e);
    void                        _process_service_status(misc::shared_ptr<io::data> const& e);
    void                        _process_state(misc::shared_ptr<io::data> const& e);
    void                        _process_log_issue(misc::shared_ptr<io::data> const& e);
    template                    <typename T>
    void                        _update_on_none_insert(
                                  database::mysql_stmt& ins_stmt,
                                  database::mysql_stmt& up_stmt,
                                  T& t,
                                  int thread_id = -1);
    void                        _update_timestamp(unsigned int instance_id);
    void                        _get_all_outdated_instances_from_db();
    void                        _update_hosts_and_services_of_unresponsive_instances();
    void                        _update_hosts_and_services_of_instance(unsigned int id, bool responsive);

    static void (stream::* const _correlation_processing_table[])(misc::shared_ptr<io::data> const&);
    static void (stream::* const _neb_processing_table[])(misc::shared_ptr<io::data> const&);
    mysql                       _mysql;

    // Cache
    database::mysql_stmt        _acknowledgement_insupdate;
    database::mysql_stmt        _comment_insupdate;
    database::mysql_stmt        _custom_variable_insupdate;
    database::mysql_stmt        _custom_variable_delete;
    database::mysql_stmt        _custom_variable_status_update;
    database::mysql_stmt        _downtime_insupdate;
    database::mysql_stmt        _empty_host_groups_delete;
    database::mysql_stmt        _empty_service_groups_delete;
    database::mysql_stmt        _event_handler_insupdate;
    database::mysql_stmt        _flapping_status_insupdate;
    database::mysql_stmt        _host_insupdate;
    database::mysql_stmt        _host_check_update;
    database::mysql_stmt        _host_dependency_insupdate;
    database::mysql_stmt        _host_group_insupdate;
    database::mysql_stmt        _host_group_member_insert;
    database::mysql_stmt        _host_group_member_delete;
    database::mysql_stmt        _host_parent_insert;
    database::mysql_stmt        _host_parent_select;
    database::mysql_stmt        _host_parent_delete;
    database::mysql_stmt        _host_state_insupdate;
    database::mysql_stmt        _host_status_update;
    database::mysql_stmt        _instance_insupdate;
    database::mysql_stmt        _instance_status_update;
    database::mysql_stmt        _issue_insupdate;
    database::mysql_stmt        _issue_parent_insert;
    database::mysql_stmt        _issue_parent_update;
    database::mysql_stmt        _log_insert;
    database::mysql_stmt        _module_insert;
    database::mysql_stmt        _service_insupdate;
    database::mysql_stmt        _service_check_update;
    database::mysql_stmt        _service_dependency_insupdate;
    database::mysql_stmt        _service_group_insupdate;
    database::mysql_stmt        _service_group_member_insert;
    database::mysql_stmt        _service_group_member_delete;
    database::mysql_stmt        _service_state_insupdate;
    database::mysql_stmt        _service_status_update;
    std::set<unsigned int>      _cache_deleted_instance_id;
    cleanup                     _cleanup_thread;
    int                         _ack_events;
    int                         _pending_events;
    bool                        _with_state_events;
    unsigned int                _instance_timeout;

    std::map<unsigned int, stored_timestamp>
                                _stored_timestamps;
    timestamp                   _oldest_timestamp;
    std::map<unsigned int, unsigned int>
                                _cache_host_instance;
    std::map<unsigned int, size_t>
                                _cache_hst_cmd;
    std::map<std::pair<unsigned int, unsigned int>, size_t>
                                _cache_svc_cmd;
    mutable std::mutex          _stat_mutex;
    bool                        _enable_cmd_cache;
  };
}

CCB_END()

#endif // !CCB_SQL_STREAM_HH
