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

#  include <deque>
#  include <map>
#  include <QString>
#  include <set>
#  include <vector>
#  include <string>
#  include "com/centreon/broker/database.hh"
#  include "com/centreon/broker/database_query.hh"
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
  class            stream : public io::stream {
  public:
                   stream(
                     database_config const& dbcfg,
                     unsigned int cleanup_check_interval,
                     unsigned int instance_timeout,
                     bool with_state_events);
                   ~stream();
    int            flush();
    bool           read(std::shared_ptr<io::data>& d, time_t deadline);
    void           update();
    int            write(std::shared_ptr<io::data> const& d);

  private:
                   stream(stream const& other);
    stream&        operator=(stream const& other);
    void           _cache_clean();
    void           _cache_create();
    void           _cache_instance_host_clean(unsigned int instance_id);
    void           _cache_instance_host_create();
    void           _clean_empty_host_groups();
    void           _clean_empty_service_groups();
    void           _clean_tables(unsigned int instance_id);
    bool           _is_valid_poller(unsigned int poller_id);
    template <typename T>
    void           _prepare_select(
                     database_query& st,
                     std::string const& table_name);
    void           _process_acknowledgement(std::shared_ptr<io::data> const& e);
    void           _process_comment(std::shared_ptr<io::data> const& e);
    void           _process_custom_variable(std::shared_ptr<io::data> const& e);
    void           _process_custom_variable_status(std::shared_ptr<io::data> const& e);
    void           _process_downtime(std::shared_ptr<io::data> const& e);
    void           _process_engine(std::shared_ptr<io::data> const& e);
    void           _process_event_handler(std::shared_ptr<io::data> const& e);
    void           _process_flapping_status(std::shared_ptr<io::data> const& e);
    void           _process_host(std::shared_ptr<io::data> const& e);
    void           _process_host_check(std::shared_ptr<io::data> const& e);
    void           _process_host_dependency(std::shared_ptr<io::data> const& e);
    void           _process_host_group(std::shared_ptr<io::data> const& e);
    void           _process_host_group_member(std::shared_ptr<io::data> const& e);
    void           _process_host_parent(std::shared_ptr<io::data> const& e);
    void           _process_host_state(std::shared_ptr<io::data> const& e);
    void           _process_host_status(std::shared_ptr<io::data> const& e);
    void           _process_instance(std::shared_ptr<io::data> const& e);
    void           _process_instance_configuration(std::shared_ptr<io::data> const& e);
    void           _process_instance_status(std::shared_ptr<io::data> const& e);
    void           _process_issue(std::shared_ptr<io::data> const& e);
    void           _process_issue_parent(std::shared_ptr<io::data> const& e);
    void           _process_log(std::shared_ptr<io::data> const& e);
    void           _process_module(std::shared_ptr<io::data> const& e);
    void           _process_notification(std::shared_ptr<io::data> const& e);
    void           _process_service(std::shared_ptr<io::data> const& e);
    void           _process_service_check(std::shared_ptr<io::data> const& e);
    void           _process_service_dependency(std::shared_ptr<io::data> const& e);
    void           _process_service_group(std::shared_ptr<io::data> const& e);
    void           _process_service_group_member(std::shared_ptr<io::data> const& e);
    void           _process_service_state(std::shared_ptr<io::data> const& e);
    void           _process_service_status(std::shared_ptr<io::data> const& e);
    void           _process_state(std::shared_ptr<io::data> const& e);
    void           _process_log_issue(std::shared_ptr<io::data> const& e);
    void           _process_responsive_instance(std::shared_ptr<io::data> const& e);
    template       <typename T>
    void           _update_on_none_insert(
                     database_query& ins,
                     database_query& up,
                     T& t);
    void           _update_timestamp(unsigned int instance_id);
    void           _get_all_outdated_instances_from_db();
    void           _update_hosts_and_services_of_unresponsive_instances();
    void           _update_hosts_and_services_of_instance(unsigned int id, bool responsive);

    static void (stream::* const _correlation_processing_table[])(std::shared_ptr<io::data> const&);
    static void (stream::* const _neb_processing_table[])(std::shared_ptr<io::data> const&);
    database       _db;
    database_query _acknowledgement_insert;
    database_query _acknowledgement_update;
    database_query _comment_insert;
    database_query _comment_update;
    database_query _custom_variable_insert;
    database_query _custom_variable_update;
    database_query _custom_variable_delete;
    database_query _custom_variable_status_update;
    database_query _downtime_insert;
    database_query _downtime_update;
    database_query _empty_host_groups_delete;
    database_query _empty_service_groups_delete;
    database_query _event_handler_insert;
    database_query _event_handler_update;
    database_query _flapping_status_insert;
    database_query _flapping_status_update;
    database_query _host_insert;
    database_query _host_update;
    database_query _host_check_update;
    database_query _host_dependency_insert;
    database_query _host_dependency_update;
    database_query _host_group_insert;
    database_query _host_group_update;
    database_query _host_group_member_insert;
    database_query _host_group_member_delete;
    database_query _host_parent_insert;
    database_query _host_parent_select;
    database_query _host_parent_delete;
    database_query _host_state_insert;
    database_query _host_state_update;
    database_query _host_status_update;
    database_query _instance_insert;
    database_query _instance_update;
    database_query _instance_status_update;
    database_query _issue_insert;
    database_query _issue_select;
    database_query _issue_update;
    database_query _issue_parent_insert;
    database_query _issue_parent_update;
    database_query _log_insert;
    database_query _module_insert;
    database_query _notification_insert;
    database_query _notification_update;
    database_query _service_insert;
    database_query _service_update;
    database_query _service_check_update;
    database_query _service_dependency_insert;
    database_query _service_dependency_update;
    database_query _service_group_insert;
    database_query _service_group_update;
    database_query _service_group_member_insert;
    database_query _service_group_member_delete;
    database_query _service_state_insert;
    database_query _service_state_update;
    database_query _service_status_update;
    std::set<unsigned int>      _cache_deleted_instance_id;
    cleanup                     _cleanup_thread;
    int                         _pending_events;
    bool                        _with_state_events;
    unsigned int                _instance_timeout;

    std::map<unsigned int, stored_timestamp>
                                _stored_timestamps;
    timestamp                   _oldest_timestamp;
    std::multimap<unsigned int, unsigned int>
                                _cache_instance_host;
    std::map<unsigned int, unsigned int>
                                _cache_hst_cmd;
    std::map<std::pair<unsigned int, unsigned int>, unsigned int>
                                _cache_svc_cmd;
  };
}

CCB_END()

#endif // !CCB_SQL_STREAM_HH
