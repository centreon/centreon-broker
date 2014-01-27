/*
** Copyright 2011-2013 Merethis
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
#  include <memory>
#  include <QPair>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QString>
#  include <QVector>
#  include <set>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/sql/cleanup.hh"

CCB_BEGIN()

namespace        sql {
  /**
   *  @class stream stream.hh "com/centreon/broker/sql/stream.hh"
   *  @brief SQL stream.
   *
   *  Stream events into SQL database.
   */
  class          stream : public io::stream {
  public:
                 stream(
                   QString const& type,
                   QString const& host,
                   unsigned short port,
                   QString const& user,
                   QString const& password,
                   QString const& db,
                   unsigned int queries_per_transaction,
                   unsigned int cleanup_check_interval,
                   bool check_replication,
                   bool with_state_events);
                 stream(stream const& s);
                 ~stream();
    static void  initialize();
    void         process(bool in = false, bool out = false);
    void         read(misc::shared_ptr<io::data>& d);
    void         update();
    unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
    stream&      operator=(stream const& s);
    void         _cache_clean();
    void         _cache_create();
    void         _clean_tables(int instance_id);
    void         _execute(QString const& query);
    void         _execute(QSqlQuery& query);
    template     <typename T>
    bool         _insert(T const& t);
    void         _prepare();
    template     <typename T>
    bool         _prepare_insert(std::auto_ptr<QSqlQuery>& st);
    template     <typename T>
    bool         _prepare_update(
                   std::auto_ptr<QSqlQuery>& st,
                   QVector<QPair<QString, bool> > const& id);
    void         _process_acknowledgement(misc::shared_ptr<io::data> const& e);
    void         _process_comment(misc::shared_ptr<io::data> const& e);
    void         _process_custom_variable(misc::shared_ptr<io::data> const& e);
    void         _process_custom_variable_status(misc::shared_ptr<io::data> const& e);
    void         _process_downtime(misc::shared_ptr<io::data> const& e);
    void         _process_engine(misc::shared_ptr<io::data> const& e);
    void         _process_event_handler(misc::shared_ptr<io::data> const& e);
    void         _process_flapping_status(misc::shared_ptr<io::data> const& e);
    void         _process_host(misc::shared_ptr<io::data> const& e);
    void         _process_host_check(misc::shared_ptr<io::data> const& e);
    void         _process_host_dependency(misc::shared_ptr<io::data> const& e);
    void         _process_host_group(misc::shared_ptr<io::data> const& e);
    void         _process_host_group_member(misc::shared_ptr<io::data> const& e);
    void         _process_host_parent(misc::shared_ptr<io::data> const& e);
    void         _process_host_state(misc::shared_ptr<io::data> const& e);
    void         _process_host_status(misc::shared_ptr<io::data> const& e);
    void         _process_instance(misc::shared_ptr<io::data> const& e);
    void         _process_instance_status(misc::shared_ptr<io::data> const& e);
    void         _process_issue(misc::shared_ptr<io::data> const& e);
    void         _process_issue_parent(misc::shared_ptr<io::data> const& e);
    void         _process_log(misc::shared_ptr<io::data> const& e);
    void         _process_module(misc::shared_ptr<io::data> const& e);
    void         _process_notification(misc::shared_ptr<io::data> const& e);
    void         _process_service(misc::shared_ptr<io::data> const& e);
    void         _process_service_check(misc::shared_ptr<io::data> const& e);
    void         _process_service_dependency(misc::shared_ptr<io::data> const& e);
    void         _process_service_group(misc::shared_ptr<io::data> const& e);
    void         _process_service_group_member(misc::shared_ptr<io::data> const& e);
    void         _process_service_state(misc::shared_ptr<io::data> const& e);
    void         _process_service_status(misc::shared_ptr<io::data> const& e);
    void         _unprepare();
    template     <typename T>
    void         _update_on_none_insert(
                   QSqlQuery& ins,
                   QSqlQuery& up,
                   T& t);
    void         _write_logs();

    static void (stream::* const _correlation_processing_table[])(misc::shared_ptr<io::data> const&);
    static void (stream::* const _neb_processing_table[])(misc::shared_ptr<io::data> const&);
    std::auto_ptr<QSqlQuery>    _acknowledgement_insert;
    std::auto_ptr<QSqlQuery>    _acknowledgement_update;
    std::auto_ptr<QSqlQuery>    _comment_insert;
    std::auto_ptr<QSqlQuery>    _comment_update;
    std::auto_ptr<QSqlQuery>    _custom_variable_insert;
    std::auto_ptr<QSqlQuery>    _custom_variable_update;
    std::auto_ptr<QSqlQuery>    _custom_variable_status_update;
    std::auto_ptr<QSqlQuery>    _downtime_insert;
    std::auto_ptr<QSqlQuery>    _downtime_update;
    std::auto_ptr<QSqlQuery>    _event_handler_insert;
    std::auto_ptr<QSqlQuery>    _event_handler_update;
    std::auto_ptr<QSqlQuery>    _flapping_status_insert;
    std::auto_ptr<QSqlQuery>    _flapping_status_update;
    std::auto_ptr<QSqlQuery>    _host_insert;
    std::auto_ptr<QSqlQuery>    _host_update;
    std::auto_ptr<QSqlQuery>    _host_check_update;
    std::auto_ptr<QSqlQuery>    _host_dependency_insert;
    std::auto_ptr<QSqlQuery>    _host_dependency_update;
    std::auto_ptr<QSqlQuery>    _host_group_insert;
    std::auto_ptr<QSqlQuery>    _host_group_update;
    std::auto_ptr<QSqlQuery>    _host_state_insert;
    std::auto_ptr<QSqlQuery>    _host_state_update;
    std::auto_ptr<QSqlQuery>    _host_status_update;
    std::auto_ptr<QSqlQuery>    _instance_insert;
    std::auto_ptr<QSqlQuery>    _instance_update;
    std::auto_ptr<QSqlQuery>    _instance_status_update;
    std::auto_ptr<QSqlQuery>    _issue_insert;
    std::auto_ptr<QSqlQuery>    _issue_select;
    std::auto_ptr<QSqlQuery>    _issue_update;
    std::auto_ptr<QSqlQuery>    _issue_parent_insert;
    std::auto_ptr<QSqlQuery>    _issue_parent_update;
    std::auto_ptr<QSqlQuery>    _notification_insert;
    std::auto_ptr<QSqlQuery>    _notification_update;
    std::auto_ptr<QSqlQuery>    _service_insert;
    std::auto_ptr<QSqlQuery>    _service_update;
    std::auto_ptr<QSqlQuery>    _service_check_update;
    std::auto_ptr<QSqlQuery>    _service_dependency_insert;
    std::auto_ptr<QSqlQuery>    _service_dependency_update;
    std::auto_ptr<QSqlQuery>    _service_group_insert;
    std::auto_ptr<QSqlQuery>    _service_group_update;
    std::auto_ptr<QSqlQuery>    _service_state_insert;
    std::auto_ptr<QSqlQuery>    _service_state_update;
    std::auto_ptr<QSqlQuery>    _service_status_update;
    std::set<unsigned int>      _cache_deleted_instance_id;
    cleanup                     _cleanup_thread;
    std::auto_ptr<QSqlDatabase> _db;
    std::deque<misc::shared_ptr<io::data> >
                                _log_queue;
    bool                        _process_out;
    unsigned int                _queries_per_transaction;
    unsigned int                _transaction_queries;
    bool                        _with_state_events;
  };
}

CCB_END()

#endif // !CCB_SQL_STREAM_HH
