/*
** Copyright 2009-2014 Centreon
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

#include "com/centreon/broker/notification/state.hh"
#include "com/centreon/broker/notification/utilities/data_loggers.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

#include "com/centreon/broker/notification/builders/command_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/composed_command_builder.hh"
#include "com/centreon/broker/notification/builders/composed_contact_builder.hh"
#include "com/centreon/broker/notification/builders/composed_dependency_builder.hh"
#include "com/centreon/broker/notification/builders/composed_node_builder.hh"
#include "com/centreon/broker/notification/builders/composed_notification_method_builder.hh"
#include "com/centreon/broker/notification/builders/composed_notification_rule_builder.hh"
#include "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/global_macro_builder.hh"
#include "com/centreon/broker/notification/builders/node_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/node_set_builder.hh"
#include "com/centreon/broker/notification/builders/notification_method_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/notification_rule_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/notification_rule_by_node_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_linker.hh"
#include "com/centreon/broker/time/timeperiod.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
state::state() : _state_mutex(QReadWriteLock::Recursive) {}

/**
 *  Copy constructor.
 *
 *  @param obj  The obj to copy.
 */
state::state(state const& obj) {
  state::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param obj  The obj to copy.
 *
 *  @return     A reference to this object.
 */
state& state::operator=(state const& obj) {
  if (this != &obj) {
    _nodes = obj._nodes;
    _node_by_id = obj._node_by_id;
    _commands = obj._commands;
    _contacts = obj._contacts;
    _contact_infos = obj._contact_infos;
    _dependency_by_child_id = obj._dependency_by_child_id;
    _dependency_by_parent_id = obj._dependency_by_parent_id;
    _timeperiod_by_id = obj._timeperiod_by_id;
    _notification_methods = obj._notification_methods;
    _notification_rules_by_node = obj._notification_rules_by_node;
    _notification_rule_by_id = obj._notification_rule_by_id;
    _date_format = obj._date_format;
    _global_constant_macros = obj._global_constant_macros;
  }
  return (*this);
}

/**
 *  Get the objects from the db.
 */
void state::update_objects_from_db(mysql& centreon_db) {
  // Acquire mutex.
  QWriteLocker lock(&_state_mutex);

  // Remove old objects.
  _nodes.clear();
  _node_by_id.clear();
  _commands.clear();
  _contacts.clear();
  _contact_infos.clear();
  _dependency_by_child_id.clear();
  _dependency_by_parent_id.clear();
  _timeperiod_by_id.clear();
  _notification_methods.clear();
  _notification_rules_by_node.clear();
  _notification_rule_by_id.clear();
  _global_constant_macros.clear();

  // Get new objects
  {
    // Get nodes.
    node_loader node;
    composed_node_builder composed;
    node_set_builder set_builder(_nodes);
    node_by_node_id_builder by_node_id_builder(_node_by_id);
    composed.push_back(set_builder);
    composed.push_back(by_node_id_builder);
    node.load(&centreon_db, &composed);
  }
  {
    // Get commands.
    command_loader command;
    composed_command_builder composed;
    command_by_id_builder by_id_builder(_commands);
    composed.push_back(by_id_builder);
    command.load(&centreon_db, &composed);
  }
  {
    // Get contacts.
    contact_loader contact;
    composed_contact_builder composed;
    contact_by_id_builder by_id_builder(_contacts, _contact_infos);
    composed.push_back(by_id_builder);
    contact.load(&centreon_db, &composed);
  }
  {
    // Get dependencies.
    dependency_loader dependency;
    composed_dependency_builder composed;
    dependency_by_node_id_builder by_node_builder(_dependency_by_child_id,
                                                  _dependency_by_parent_id);
    composed.push_back(by_node_builder);
    dependency.load(&centreon_db, &composed);
  }
  {
    // Get timeperiods.
    timeperiod_loader timeperiod;
    composed_timeperiod_builder composed;
    timeperiod_by_id_builder by_id_builder(_timeperiod_by_id);
    timeperiod_linker linker;
    composed.push_back(by_id_builder);
    composed.push_back(linker);
    timeperiod.load(&centreon_db, &composed);
  }
  {
    // Get notification methods.
    notification_method_loader nml;
    composed_notification_method_builder composed;
    notification_method_by_id_builder by_id_builder(_notification_methods);
    composed.push_back(by_id_builder);
    nml.load(&centreon_db, &composed);
  }
  {
    // Get notification rules.
    notification_rule_loader nrl;
    composed_notification_rule_builder composed;
    notification_rule_by_node_builder by_node_builder(
        _notification_rules_by_node);
    notification_rule_by_id_builder by_id_builder(_notification_rule_by_id);
    composed.push_back(by_node_builder);
    composed.push_back(by_id_builder);
    nrl.load(&centreon_db, &composed);
  }
  {
    // Get global constant macros.
    macro_loader ml;
    global_macro_builder builder(_global_constant_macros, _date_format);
    ml.load(&centreon_db, &builder);
  }

  // Debug logging for all the data loaded.
#ifndef NDEBUG
  // data_logger::log_container("_nodes", _nodes);
  // data_logger::log_container("_node_by_id", _node_by_id);
  // data_logger::log_container("_commands", _commands);
  // data_logger::log_container("_contacts", _contacts);
  // data_logger::log_container("_dependency_by_child_id",
  //                            _dependency_by_child_id);
  // data_logger::log_container("_dependency_by_parent_id",
  //                            _dependency_by_parent_id);
  // data_logger::log_container("_timeperiod_by_id", _timeperiod_by_id);
  // data_logger::log_container("_global_constant_macro",
  // _global_constant_macros);
  // data_logger::log_container("_notification_methods", _notification_methods);
  // data_logger::log_container("_notification_rules_by_node",
  // _notification_rules_by_node
  // );
#endif  //! NDEBUG
}

/**
 *  Get a node by its node id.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return        A node::ptr to the node, or a null node::ptr.
 */
node::ptr state::get_node_by_id(node_id id) const {
  return (_node_by_id.value(id));
}

/**
 *  Get all the notification rules associated to a node.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return        A list of notification_rule::ptr associated to this node.
 */
QList<notification_rule::ptr> state::get_notification_rules_by_node(
    node_id id) const {
  return (_notification_rules_by_node.values(id));
}

/**
 *  Get the notification rule associated to a id.
 *
 *  @param[in] id  The id of the notification rule.
 *
 *  @return        A notification_rule::ptr to the notification rule, or a null
 * ptr.
 */
notification_rule::ptr state::get_notification_rule_by_id(uint32_t id) const {
  return (_notification_rule_by_id.value(id));
}

/**
 *  Get a notification method from its id.
 *
 *  @param[in] id  The notification method id.
 *
 *  @return        A notification_method::ptr the notification method, or a null
 * notification_method::ptr.
 */
notification_method::ptr state::get_notification_method_by_id(
    uint32_t id) const {
  return (_notification_methods.value(id));
}

/**
 *  Get a timeperiod by its id.
 *
 *  @param[in] id    The id of the timeperiod.
 *
 *  @return          A timeperiod::ptr to the timeperiod, or a null
 * timeperiod::ptr.
 */
::com::centreon::broker::time::timeperiod::ptr state::get_timeperiod_by_id(
    uint32_t id) const {
  return (_timeperiod_by_id.value(id));
}

/**
 *  Get a contact by its id.
 *
 *  @param[in] id   The id of the contact.
 *
 *  @return         A contact::ptr to the contact, or a null contact::ptr.
 */
objects::contact::ptr state::get_contact_by_id(uint32_t id) const {
  return (_contacts.value(id));
}

/**
 *  Get the infos of a contact by its contact id.
 *
 *  @param[in] id  The id of the contact.
 *
 *  @return        A map of the contact infos.
 */
QHash<std::string, std::string> state::get_contact_infos(uint32_t id) const {
  return (_contact_infos.value(id));
}

/**
 *  Get a command by its id.
 *
 *  @param[în] id  The id of the command.
 *
 *  @return         A command::ptr to the command, or a null command::ptr.
 */
objects::command::ptr state::get_command_by_id(uint32_t id) const {
  return (_commands.value(id));
}

/**
 *  Get a read lock on this state.
 *
 *  @return  A QReadLocker locking this state.
 */
std::unique_ptr<QReadLocker> state::read_lock() {
  return (std::unique_ptr<QReadLocker>(new QReadLocker(&_state_mutex)));
}

/**
 *  Get a write lock on this state.
 *
 *  @return  A QWriteLocker locking this state.
 */
std::unique_ptr<QWriteLocker> state::write_lock() {
  return (std::unique_ptr<QWriteLocker>(new QWriteLocker(&_state_mutex)));
}

/**
 *  Get the global macros from the state.
 *
 *  @return  The global macros.
 */
QHash<std::string, std::string> const& state::get_global_macros() const {
  return (_global_constant_macros);
}

/**
 *  Get the format of the date (ie US, Euro, Iso...)
 *
 *  @return  The format of the date.
 */
int state::get_date_format() const {
  return (_date_format);
}

/**
 *  Get all the services of an host.
 *
 *  @param[in] id  The id of the host.
 *
 *  @return        All the services of the host.
 */
QList<objects::node::ptr> state::get_all_services_of_host(
    objects::node_id id) const {
  if (!id.is_host())
    return (QList<objects::node::ptr>());
  QList<objects::node::ptr> list;
  for (QHash<objects::node_id, objects::node::ptr>::const_iterator
           it(_node_by_id.begin()),
       end(_node_by_id.end());
       it != end; ++it)
    if (it.key().get_host_id() == id.get_host_id() && it.key().is_service())
      list.push_back(*it);
  return (list);
}

/**
 *  Get all the hosts in a given state.
 *
 *  @param[in] state  The state. -1 for all that are not up.
 *
 *  @return  The hosts in the given state.
 */
QList<objects::node::ptr> state::get_all_hosts_in_state(short state) const {
  QList<objects::node::ptr> list;
  for (QHash<objects::node_id, objects::node::ptr>::const_iterator
           it(_node_by_id.begin()),
       end(_node_by_id.end());
       it != end; ++it)
    if (state == -1 && it.key().is_host() &&
        (*it)->get_hard_state() != objects::node_state::host_up)
      list.push_back(*it);
    else if (it.key().is_host() &&
             (*it)->get_hard_state() == objects::node_state(state))
      list.push_back(*it);
  return (list);
}

/**
 *  Get all the services in a given state.
 *
 *  @param[in] state  The state. -1 for all that are not ok.
 *
 *  @return  The services in the given state.
 */
QList<objects::node::ptr> state::get_all_services_in_state(short state) const {
  QList<objects::node::ptr> list;
  for (QHash<objects::node_id, objects::node::ptr>::const_iterator
           it(_node_by_id.begin()),
       end(_node_by_id.end());
       it != end; ++it)
    if (state == -1 && it.key().is_service() &&
        (*it)->get_hard_state() != objects::node_state::service_ok)
      list.push_back(*it);
    else if (it.key().is_service() &&
             (*it)->get_hard_state() == objects::node_state(state))
      list.push_back(*it);
  return (list);
}
