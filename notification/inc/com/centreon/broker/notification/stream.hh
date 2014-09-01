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
#  include "com/centreon/broker/notification/loaders/command_loader.hh"
#  include "com/centreon/broker/notification/loaders/contact_loader.hh"
#  include "com/centreon/broker/notification/loaders/dependency_loader.hh"
#  include "com/centreon/broker/notification/loaders/escalation_loader.hh"
#  include "com/centreon/broker/notification/loaders/node_loader.hh"
#  include "com/centreon/broker/notification/loaders/timeperiod_loader.hh"
#  include "com/centreon/broker/notification/loaders/acknowledgement_loader.hh"
#  include "com/centreon/broker/notification/loaders/downtime_loader.hh"
#  include "com/centreon/broker/notification/builders/node_set_builder.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class stream stream.hh "com/centreon/broker/notification/stream.hh"
   *  @brief Notification stream.
   *
   *  Stream events into notification database.
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
                   QString const& centreon_storage_db,
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
    std::auto_ptr<QSqlDatabase>           _centreon_db;
    std::auto_ptr<QSqlDatabase>           _centreon_storage_db;
    bool                                  _process_out;
    unsigned int                          _queries_per_transaction;
    unsigned int                          _transaction_queries;
    bool                                  _with_state_events;
    unsigned int                          _instance_timeout;

    QSet<node_id>                         _nodes;
    QMultiHash<node_id, acknowledgement::ptr> _acks;
    QHash<std::string, command::ptr>      _commands;
    QHash<std::string, contact::ptr>      _contact_by_name;
    QMultiHash<node_id, contact::ptr>     _contacts;
    QMultiHash<std::string, contact::ptr> _contact_by_command;
    QMultiHash<node_id, dependency::ptr>  _dependency_by_child_id;
    QMultiHash<node_id, dependency::ptr>  _dependency_by_parent_id;
    QMultiHash<node_id, downtime::ptr>    _downtimes;
    QMultiHash<node_id, escalation::ptr>  _escalations;
    QHash<std::string, timeperiod::ptr>   _timeperiod_by_name;

    void                        _open_db(std::auto_ptr<QSqlDatabase>& db,
                                         QString const& t,
                                         QString const& host,
                                         unsigned short port,
                                         QString const& user,
                                         QString const& password,
                                         QString const& db_name,
                                         QString const& id,
                                         bool check_replication);
    void                        _clone_db(std::auto_ptr<QSqlDatabase>& db,
                                          std::auto_ptr<QSqlDatabase> const& db_to_clone,
                                          QString const& id);
    void                        _update_objects_from_db();
    void                        _process_service_status_event(neb::service_status& event);
    void                        _process_host_status_event(neb::host_status& event);
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_STREAM_HH
