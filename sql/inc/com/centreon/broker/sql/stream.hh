/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_SQL_STREAM_HH
#  define CCB_SQL_STREAM_HH

#  include <deque>
#  include <map>
#  include <memory>
#  include <QString>
#  include <set>
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
                     std::string const& type,
                     std::string const& host,
                     unsigned short port,
                     std::string const& user,
                     std::string const& password,
                     std::string const& db,
                     unsigned int queries_per_transaction,
                     unsigned int cleanup_check_interval,
                     unsigned int instance_timeout,
                     bool check_replication,
                     bool with_state_events);
                   ~stream();
    static void    initialize();
    void           process(bool in = false, bool out = false);
    void           read(misc::shared_ptr<io::data>& d);
    void           update();
    unsigned int   write(misc::shared_ptr<io::data> const& d);

  private:
                   stream(stream const& other);
    stream&        operator=(stream const& other);
    void           _cache_clean();
    void           _cache_create();
    void           _clean_tables(int instance_id);
    void           _prepare();
    template       <typename T>
    void           _prepare_insert(
                     database_query& st,
                     std::string const& table_name);
    template       <typename T>
    void           _prepare_update(
                     database_query& st,
                     std::string const& table_name,
                     std::map<std::string, bool> const& id);
    template       <typename T>
    void           _prepare_delete(
                     database_query& st,
                     std::string const& table_name,
                     std::map<std::string, bool> const& id);
    template <typename T>
    void           _prepare_select(
                     database_query& st,
                     std::string const& table_name);
    void           _process_acknowledgement(misc::shared_ptr<io::data> const& e);
    void           _process_comment(misc::shared_ptr<io::data> const& e);
    void           _process_custom_variable(misc::shared_ptr<io::data> const& e);
    void           _process_custom_variable_status(misc::shared_ptr<io::data> const& e);
    void           _process_downtime(misc::shared_ptr<io::data> const& e);
    void           _process_engine(misc::shared_ptr<io::data> const& e);
    void           _process_event_handler(misc::shared_ptr<io::data> const& e);
    void           _process_flapping_status(misc::shared_ptr<io::data> const& e);
    void           _process_host(misc::shared_ptr<io::data> const& e);
    void           _process_host_check(misc::shared_ptr<io::data> const& e);
    void           _process_host_dependency(misc::shared_ptr<io::data> const& e);
    void           _process_host_group(misc::shared_ptr<io::data> const& e);
    void           _process_host_group_member(misc::shared_ptr<io::data> const& e);
    void           _process_host_parent(misc::shared_ptr<io::data> const& e);
    void           _process_host_state(misc::shared_ptr<io::data> const& e);
    void           _process_host_status(misc::shared_ptr<io::data> const& e);
    void           _process_instance(misc::shared_ptr<io::data> const& e);
    void           _process_instance_status(misc::shared_ptr<io::data> const& e);
    void           _process_issue(misc::shared_ptr<io::data> const& e);
    void           _process_issue_parent(misc::shared_ptr<io::data> const& e);
    void           _process_log(misc::shared_ptr<io::data> const& e);
    void           _process_module(misc::shared_ptr<io::data> const& e);
    void           _process_notification(misc::shared_ptr<io::data> const& e);
    void           _process_service(misc::shared_ptr<io::data> const& e);
    void           _process_service_check(misc::shared_ptr<io::data> const& e);
    void           _process_service_dependency(misc::shared_ptr<io::data> const& e);
    void           _process_service_group(misc::shared_ptr<io::data> const& e);
    void           _process_service_group_member(misc::shared_ptr<io::data> const& e);
    void           _process_service_state(misc::shared_ptr<io::data> const& e);
    void           _process_service_status(misc::shared_ptr<io::data> const& e);
    template       <typename T>
    void           _update_on_none_insert(
                     database_query& ins,
                     database_query& up,
                     T& t);
    void           _write_logs();
    void           _update_timestamp(unsigned int instance_id);
    void           _get_all_outdated_instances_from_db();
    void           _update_hosts_and_services_of_unresponsive_instances();
    void           _update_hosts_and_services_of_instance(unsigned int id, bool responsive);

    static void (stream::* const _correlation_processing_table[])(misc::shared_ptr<io::data> const&);
    static void (stream::* const _neb_processing_table[])(misc::shared_ptr<io::data> const&);
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
    database_query _host_parent_select;
    database_query _host_parent_insert;
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
    database_query _service_state_insert;
    database_query _service_state_update;
    database_query _service_status_update;
    std::set<unsigned int>      _cache_deleted_instance_id;
    cleanup                     _cleanup_thread;
    std::deque<misc::shared_ptr<io::data> >
                                _log_queue;
    int                         _pending_events;
    bool                        _process_out;
    bool                        _with_state_events;
    unsigned int                _instance_timeout;

    std::map<unsigned int, stored_timestamp>
                                _stored_timestamps;
    timestamp                   _oldest_timestamp;
  };
}

CCB_END()

#endif // !CCB_SQL_STREAM_HH
