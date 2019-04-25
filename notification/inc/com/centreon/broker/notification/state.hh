/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_STATE_HH
#  define CCB_NOTIFICATION_STATE_HH

#  include <ctime>
#  include <memory>
#  include <QSet>
#  include <QPair>
#  include <QString>
#  include <QVector>
#  include <QReadWriteLock>
#  include "com/centreon/broker/mysql.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/loaders/command_loader.hh"
#  include "com/centreon/broker/notification/loaders/contact_loader.hh"
#  include "com/centreon/broker/notification/loaders/dependency_loader.hh"
#  include "com/centreon/broker/notification/loaders/node_loader.hh"
#  include "com/centreon/broker/notification/loaders/timeperiod_loader.hh"
#  include "com/centreon/broker/notification/loaders/macro_loader.hh"
#  include "com/centreon/broker/notification/loaders/notification_method_loader.hh"
#  include "com/centreon/broker/notification/loaders/notification_rule_loader.hh"
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

    void              update_objects_from_db(mysql& centreon_db);

    std::unique_ptr<QReadLocker>
                      read_lock();
    std::unique_ptr<QWriteLocker>
                      write_lock();

    objects::node::ptr
                      get_node_by_id(objects::node_id) const;
    time::timeperiod::ptr
                      get_timeperiod_by_id(unsigned int id) const;
    QList<objects::notification_rule::ptr>
                      get_notification_rules_by_node(objects::node_id id) const;
    objects::notification_method::ptr
                      get_notification_method_by_id(unsigned int id) const;
    objects::notification_rule::ptr
                      get_notification_rule_by_id(unsigned int id) const;
    objects::contact::ptr
                      get_contact_by_id(unsigned int id) const;
    QHash<std::string, std::string>
                      get_contact_infos(unsigned int id) const;
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
    int               get_date_format() const;

  private:
    QSet<objects::node_id>
                      _nodes;
    QHash<objects::node_id, objects::node::ptr>
                      _node_by_id;
    QHash<unsigned int, objects::command::ptr>
                      _commands;
    QHash<unsigned int, objects::contact::ptr>
                      _contacts;
    QHash<unsigned int, QHash<std::string, std::string> >
                      _contact_infos;
    QMultiHash<objects::node_id, objects::dependency::ptr>
                      _dependency_by_child_id;
    QMultiHash<objects::node_id, objects::dependency::ptr>
                      _dependency_by_parent_id;
    QHash<unsigned int, time::timeperiod::ptr>
                      _timeperiod_by_id;
    QHash<unsigned int, objects::notification_method::ptr>
                      _notification_methods;
    QMultiHash<objects::node_id, objects::notification_rule::ptr>
                      _notification_rules_by_node;
    QHash<unsigned int, objects::notification_rule::ptr>
                      _notification_rule_by_id;

    int               _date_format;
    QHash<std::string, std::string>
                      _global_constant_macros;

    QReadWriteLock    _state_mutex;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_STATE_HH
