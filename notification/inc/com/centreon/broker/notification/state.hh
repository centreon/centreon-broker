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

#ifndef CCB_NOTIFICATION_STATE_HH
#  define CCB_NOTIFICATION_STATE_HH

#  include <ctime>
#  include <memory>
#  include <QSet>
#  include <QPair>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QString>
#  include <QVector>
#  include <QReadWriteLock>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/loaders/command_loader.hh"
#  include "com/centreon/broker/notification/loaders/contact_loader.hh"
#  include "com/centreon/broker/notification/loaders/contactgroup_loader.hh"
#  include "com/centreon/broker/notification/loaders/dependency_loader.hh"
#  include "com/centreon/broker/notification/loaders/node_loader.hh"
#  include "com/centreon/broker/notification/loaders/timeperiod_loader.hh"
#  include "com/centreon/broker/notification/loaders/acknowledgement_loader.hh"
#  include "com/centreon/broker/notification/loaders/downtime_loader.hh"
#  include "com/centreon/broker/notification/loaders/macro_loader.hh"
#  include "com/centreon/broker/notification/loaders/notification_method_loader.hh"
#  include "com/centreon/broker/notification/loaders/notification_rule_loader.hh"
#  include "com/centreon/broker/notification/loaders/nodegroup_loader.hh"
#  include "com/centreon/broker/notification/builders/node_set_builder.hh"
#  include "com/centreon/broker/notification/notification_scheduler.hh"

CCB_BEGIN()

namespace             notification {
  /**
   *  @class state state.hh "com/centreon/broker/notification/state.hh"
   *  @brief Regroup the objects loaded from the database.
   */
  class               state {
  public:
                      state();
                      state(state const& obj);
    state&            operator=(state const& obj);

    void              update_objects_from_db(QSqlDatabase& centreon_db);

    std::auto_ptr<QReadLocker>
                      read_lock();
    std::auto_ptr<QWriteLocker>
                      write_lock();

    objects::node::ptr
                      get_node_by_id(objects::node_id) const;
    objects::timeperiod::ptr
                      get_timeperiod_by_id(unsigned int id) const;
    QList<objects::notification_rule::ptr>
                      get_notification_rules_by_node(objects::node_id id) const;
    objects::notification_method::ptr
                      get_notification_method_by_id(unsigned int id) const;
    objects::notification_rule::ptr
                      get_notification_rule_by_id(unsigned int id) const;
    objects::contact::ptr
                      get_contact_by_id(unsigned int id) const;
    objects::command::ptr
                      get_command_by_id(unsigned int id) const;
    QHash<std::string, std::string> const&
                      get_global_macros() const;
    QList<objects::node::ptr>
                      get_all_services_of_host(objects::node_id id) const;
    QList<objects::node::ptr>
                      get_all_hosts_in_state(short state) const;
    QList<objects::node::ptr>
                      get_all_services_in_state(short state) const;
    objects::nodegroup::ptr
                      get_nodegroup_by_name(std::string const& name) const;
    objects::contactgroup::ptr
                      get_contactgroup_by_contact_id(unsigned int contact_id) const;
    QList<unsigned int>
                      get_contacts_by_contactgroup(objects::contactgroup::ptr cnt) const;

    int               get_date_format() const;

    bool              is_node_in_downtime(objects::node_id id) const;
    bool              has_node_been_acknowledged(objects::node_id id) const;

  private:
    QSet<objects::node_id>
                      _nodes;
    QHash<objects::node_id, objects::node::ptr>
                      _node_by_id;
    QMultiHash<objects::node_id, objects::acknowledgement::ptr>
                      _acks;
    QHash<unsigned int, objects::command::ptr>
                      _commands;
    QHash<unsigned int, objects::contact::ptr>
                      _contacts;
    QMultiHash<objects::node_id, objects::dependency::ptr>
                      _dependency_by_child_id;
    QMultiHash<objects::node_id, objects::dependency::ptr>
                      _dependency_by_parent_id;
    QMultiHash<objects::node_id, objects::downtime::ptr>
                      _downtimes;
    QHash<unsigned int, objects::timeperiod::ptr>
                      _timeperiod_by_id;
    QHash<unsigned int, objects::notification_method::ptr>
                      _notification_methods;
    QMultiHash<objects::node_id, objects::notification_rule::ptr>
                      _notification_rules_by_node;
    QHash<unsigned int, objects::notification_rule::ptr>
                      _notification_rule_by_id;
    QHash<std::string, objects::nodegroup::ptr>
                      _nodegroups_by_name;
    QMultiHash<unsigned int, objects::contactgroup::ptr>
                      _contactgroups_by_contact_id;
    QMultiHash<objects::contactgroup::ptr, unsigned int>
                      _contact_id_by_contactgroups;

    int               _date_format;
    QHash<std::string, std::string>
                      _global_constant_macros;

    QReadWriteLock    _state_mutex;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_STATE_HH
