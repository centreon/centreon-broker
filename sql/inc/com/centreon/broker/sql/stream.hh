/*
** Copyright 2011-2012 Merethis
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

#ifndef CCB_SQL_STREAM_HH_
# define CCB_SQL_STREAM_HH_

# include <QHash>
# include <QMap>
# include <QPair>
# include <QScopedPointer>
# include <QSqlDatabase>
# include <QSqlQuery>
# include <QString>
# include <QVector>
# include "com/centreon/broker/io/stream.hh"
# include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        sql {
  /**
   *  @class stream stream.hh "com/centreon/broker/sql/stream.hh"
   *  @brief SQL stream.
   *
   *  Stream events into SQL database.
   */
  class          stream : public io::stream {
   private:
    static QHash<QString, void (stream::*)(io::data const&)>
                                 _processing_table;
    QScopedPointer<QSqlQuery>    _acknowledgement_insert;
    QScopedPointer<QSqlQuery>    _acknowledgement_update;
    QScopedPointer<QSqlQuery>    _comment_insert;
    QScopedPointer<QSqlQuery>    _comment_update;
    QScopedPointer<QSqlQuery>    _custom_variable_insert;
    QScopedPointer<QSqlQuery>    _custom_variable_update;
    QScopedPointer<QSqlQuery>    _custom_variable_status_update;
    QScopedPointer<QSqlQuery>    _downtime_insert;
    QScopedPointer<QSqlQuery>    _downtime_update;
    QScopedPointer<QSqlQuery>    _event_handler_insert;
    QScopedPointer<QSqlQuery>    _event_handler_update;
    QScopedPointer<QSqlQuery>    _flapping_status_insert;
    QScopedPointer<QSqlQuery>    _flapping_status_update;
    QScopedPointer<QSqlQuery>    _host_insert;
    QScopedPointer<QSqlQuery>    _host_update;
    QScopedPointer<QSqlQuery>    _host_check_update;
    QScopedPointer<QSqlQuery>    _host_dependency_insert;
    QScopedPointer<QSqlQuery>    _host_dependency_update;
    QScopedPointer<QSqlQuery>    _host_group_insert;
    QScopedPointer<QSqlQuery>    _host_group_update;
    QScopedPointer<QSqlQuery>    _host_state_insert;
    QScopedPointer<QSqlQuery>    _host_state_update;
    QScopedPointer<QSqlQuery>    _host_status_update;
    QScopedPointer<QSqlQuery>    _instance_insert;
    QScopedPointer<QSqlQuery>    _instance_update;
    QScopedPointer<QSqlQuery>    _instance_status_update;
    QScopedPointer<QSqlQuery>    _issue_insert;
    QScopedPointer<QSqlQuery>    _issue_update;
    QScopedPointer<QSqlQuery>    _issue_parent_insert;
    QScopedPointer<QSqlQuery>    _issue_parent_update;
    QScopedPointer<QSqlQuery>    _notification_insert;
    QScopedPointer<QSqlQuery>    _notification_update;
    QScopedPointer<QSqlQuery>    _service_insert;
    QScopedPointer<QSqlQuery>    _service_update;
    QScopedPointer<QSqlQuery>    _service_check_update;
    QScopedPointer<QSqlQuery>    _service_dependency_insert;
    QScopedPointer<QSqlQuery>    _service_dependency_update;
    QScopedPointer<QSqlQuery>    _service_group_insert;
    QScopedPointer<QSqlQuery>    _service_group_update;
    QScopedPointer<QSqlQuery>    _service_state_insert;
    QScopedPointer<QSqlQuery>    _service_state_update;
    QScopedPointer<QSqlQuery>    _service_status_update;
    QScopedPointer<QSqlDatabase> _db;
    bool                         _process_out;
    bool                         _with_state_events;
    stream&      operator=(stream const& s);
    void         _clean_tables(int instance_id);
    void         _execute(QString const& query);
    void         _execute(QSqlQuery& query);
    template     <typename T>
    bool         _insert(T const& t);
    void         _prepare();
    template     <typename T>
    bool         _prepare_insert(QScopedPointer<QSqlQuery>& st);
    template     <typename T>
    bool         _prepare_update(
                   QScopedPointer<QSqlQuery>& st,
                   QVector<QPair<QString, bool> > const& id);
    void         _process_acknowledgement(io::data const& e);
    void         _process_comment(io::data const& e);
    void         _process_custom_variable(io::data const& e);
    void         _process_custom_variable_status(io::data const& e);
    void         _process_downtime(io::data const& e);
    void         _process_engine(io::data const& e);
    void         _process_event_handler(io::data const& e);
    void         _process_flapping_status(io::data const& e);
    void         _process_host(io::data const& e);
    void         _process_host_check(io::data const& e);
    void         _process_host_dependency(io::data const& e);
    void         _process_host_group(io::data const& e);
    void         _process_host_group_member(io::data const& e);
    void         _process_host_parent(io::data const& e);
    void         _process_host_state(io::data const& e);
    void         _process_host_status(io::data const& e);
    void         _process_instance(io::data const& e);
    void         _process_instance_status(io::data const& e);
    void         _process_issue(io::data const& e);
    void         _process_issue_parent(io::data const& e);
    void         _process_log(io::data const& e);
    void         _process_module(io::data const& e);
    void         _process_notification(io::data const& e);
    void         _process_service(io::data const& e);
    void         _process_service_check(io::data const& e);
    void         _process_service_dependency(io::data const& e);
    void         _process_service_group(io::data const& e);
    void         _process_service_group_member(io::data const& e);
    void         _process_service_state(io::data const& e);
    void         _process_service_status(io::data const& e);
    void         _unprepare();
    template     <typename T>
    void         _update_on_none_insert(
                   QSqlQuery& ins,
                   QSqlQuery& up,
                   T& t);

   public:
                 stream(
                   QString const& type,
                   QString const& host,
                   unsigned short port,
                   QString const& user,
                   QString const& password,
                   QString const& db,
                   bool with_state_events);
                 stream(stream const& s);
                 ~stream();
    static void  initialize();
    void         process(bool in = false, bool out = false);
    QSharedPointer<io::data>
                 read();
    void         write(QSharedPointer<io::data> d);
  };
}

CCB_END()

#endif /* !CCB_SQL_STREAM_HH_ */
