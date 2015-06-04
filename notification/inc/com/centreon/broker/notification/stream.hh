/*
** Copyright 2014-2015 Merethis
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

#ifndef CCB_NOTIFICATION_STREAM_HH
#  define CCB_NOTIFICATION_STREAM_HH

#  include <vector>
#  include <deque>
#  include <memory>
#  include <QSet>
#  include <QPair>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QString>
#  include <QVector>
#  include <set>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/state.hh"
#  include "com/centreon/broker/correlation/issue_parent.hh"
#  include "com/centreon/broker/neb/acknowledgement.hh"
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/notification/process_manager.hh"
#  include "com/centreon/broker/notification/node_cache.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class stream stream.hh "com/centreon/broker/notification/stream.hh"
   *  @brief Notification stream.
   *
   *  Stream events into the notification database.
   */
  class          stream : public io::stream {
  public:
                 stream(
                   QString const& type,
                   QString const& host,
                   unsigned short port,
                   QString const& user,
                   QString const& password,
                   QString const& centreon_db,
                   unsigned int queries_per_transaction,
                   bool check_replication,
                   bool with_state_events,
                   node_cache& cache);
                 stream(stream const& s);
                 ~stream();
    static void  initialize();
    void         process(bool in = false, bool out = false);
    bool         read(
                   misc::shared_ptr<io::data>& d,
                   time_t deadline);
    void         update();
    unsigned int write(misc::shared_ptr<io::data> const& d);

  private:
    stream&      operator=(stream const& s);
    void         _open_db(
                   std::auto_ptr<QSqlDatabase>& db,
                   QString const& t,
                   QString const& host,
                   unsigned short port,
                   QString const& user,
                   QString const& password,
                   QString const& db_name,
                   QString const& id,
                   bool check_replication);
    void         _clone_db(
                   std::auto_ptr<QSqlDatabase>& db,
                   std::auto_ptr<QSqlDatabase> const& db_to_clone,
                   QString const& id);
    void         _update_objects_from_db();
    void         _process_service_status_event(
                   neb::service_status const& event);
    void         _process_host_status_event(
                   neb::host_status const& event);
    void         _process_issue_parent_event(
                   correlation::issue_parent const& event);
    void         _process_ack(
                   neb::acknowledgement const& event);
    void         _process_downtime(
                   neb::downtime const& event);

    std::auto_ptr<QSqlDatabase>
                 _centreon_db;
    std::auto_ptr<notification_scheduler>
                 _notif_scheduler;
    bool         _process_out;
    unsigned int _queries_per_transaction;
    unsigned int _transaction_queries;
    bool         _with_state_events;
    unsigned int _instance_timeout;

    state        _state;
    node_cache&  _node_cache;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_STREAM_HH
