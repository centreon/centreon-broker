/*
** Copyright 2009-2014 Merethis
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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include "com/centreon/broker/notification/state.hh"
#include "com/centreon/broker/notification/utilities/data_loggers.hh"

#include "com/centreon/broker/notification/builders/composed_acknowledgement_builder.hh"
#include "com/centreon/broker/notification/builders/composed_command_builder.hh"
#include "com/centreon/broker/notification/builders/composed_contact_builder.hh"
#include "com/centreon/broker/notification/builders/composed_dependency_builder.hh"
#include "com/centreon/broker/notification/builders/composed_downtime_builder.hh"
#include "com/centreon/broker/notification/builders/composed_node_builder.hh"
#include "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"
#include "com/centreon/broker/notification/builders/composed_notification_method_builder.hh"
#include "com/centreon/broker/notification/builders/composed_notification_rule_builder.hh"

#include "com/centreon/broker/notification/builders/acknowledgement_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/command_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/node_set_builder.hh"
#include "com/centreon/broker/notification/builders/node_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/notification_method_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/notification_rule_by_node_builder.hh"
#include "com/centreon/broker/notification/builders/notification_rule_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/nodegroup_by_name_builder.hh"
#include "com/centreon/broker/notification/builders/contactgroup_by_contact_builder.hh"
#include "com/centreon/broker/notification/builders/global_macro_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
state::state() :
  _state_mutex(QReadWriteLock::Recursive) {}

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
    _acks = obj._acks;
    _commands = obj._commands;
    _contacts = obj._contacts;
    _dependency_by_child_id = obj._dependency_by_child_id;
    _dependency_by_parent_id = obj._dependency_by_parent_id;
    _downtimes = obj._downtimes;
    _timeperiod_by_id = obj._timeperiod_by_id;
    _notification_methods = obj._notification_methods;
    _notification_rules_by_node = obj._notification_rules_by_node;
    _notification_rule_by_id = obj._notification_rule_by_id;
    _date_format = obj._date_format;
    _global_constant_macros = obj._global_constant_macros;
    _nodegroups_by_name = obj._nodegroups_by_name;
    _contactgroups_by_contact_id = obj._contactgroups_by_contact_id;
    _contact_id_by_contactgroups = obj._contact_id_by_contactgroups;
  }
  return (*this);
}

/**
 *  Get the objects from the db.
 */
void state::update_objects_from_db(QSqlDatabase& centreon_db) {
  // Acquire mutex.
  QWriteLocker lock(&_state_mutex);

  // Remove old objects.
  _nodes.clear();
  _node_by_id.clear();
  _acks.clear();
  _commands.clear();
  _contacts.clear();
  _dependency_by_child_id.clear();
  _dependency_by_parent_id.clear();
  _downtimes.clear();
  _timeperiod_by_id.clear();
  _notification_methods.clear();
  _notification_rules_by_node.clear();
  _notification_rule_by_id.clear();
  _global_constant_macros.clear();
  _nodegroups_by_name.clear();
  _contactgroups_by_contact_id.clear();
  _contact_id_by_contactgroups.clear();

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
    contact_by_id_builder by_id_builder(_contacts);
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
    composed.push_back(by_id_builder);
    timeperiod.load(&centreon_db, &composed);
  }
  // XXX
  // {
  //   // Get downtimes.
  //   downtime_loader downtime;
  //   composed_downtime_builder composed;
  //   downtime_by_node_id_builder by_node_builder(_downtimes);
  //   composed.push_back(by_node_builder);
  //   downtime.load(&centreon_db, &composed);
  // }
  // {
  //   // Get acknowledgements.
  //   acknowledgement_loader ack;
  //   composed_acknowledgement_builder composed;
  //   acknowledgement_by_node_id_builder by_node_builder(_acks);
  //   composed.push_back(by_node_builder);
  //   ack.load(&centreon_db, &composed);
  // }
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
    notification_rule_by_node_builder by_node_builder(_notification_rules_by_node);
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
  {
    // Get nodegroups.
    nodegroup_loader nl;
    nodegroup_by_name_builder builder(_nodegroups_by_name);
    nl.load(&centreon_db, &builder);
  }
  {
    // Get contactgroups.
    contactgroup_loader cl;
    contactgroup_by_contact_builder builder(
                                      _contactgroups_by_contact_id,
                                      _contact_id_by_contactgroups);
    cl.load(&centreon_db, &builder);
  }
  // Debug logging for all the data loaded.
#ifndef NDEBUG
    // data_logger::log_container("_nodes", _nodes);
    // data_logger::log_container("_node_by_id", _node_by_id);
    // data_logger::log_container("_acks", _acks);
    // data_logger::log_container("_commands", _commands);
    // data_logger::log_container("_contacts", _contacts);
    // data_logger::log_container("_dependency_by_child_id",
    //                            _dependency_by_child_id);
    // data_logger::log_container("_dependency_by_parent_id",
    //                            _dependency_by_parent_id);
    // data_logger::log_container("_downtimes", _downtimes);
    // data_logger::log_container("_timeperiod_by_id", _timeperiod_by_id);
    // data_logger::log_container("_global_constant_macro", _global_constant_macros);
    // data_logger::log_container("_notification_methods", _notification_methods);
    // data_logger::log_container("_notification_rules_by_node", _notification_rules_by_node
  // );
#endif //!NDEBUG
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
 *  @return        A notification_rule::ptr to the notification rule, or a null ptr.
 */
notification_rule::ptr state::get_notification_rule_by_id(unsigned int id) const {
  return (_notification_rule_by_id.value(id));
}


/**
 *  Get a notification method from its id.
 *
 *  @param[in] id  The notification method id.
 *
 *  @return        A notification_method::ptr the notification method, or a null notification_method::ptr.
 */
notification_method::ptr state::get_notification_method_by_id(
                                  unsigned int id) const {
  return (_notification_methods.value(id));
}

/**
 *  Get a timeperiod by its id.
 *
 *  @param[in] id    The id of the timeperiod.
 *
 *  @return          A timeperiod::ptr to the timeperiod, or a null timeperiod::ptr.
 */
timeperiod::ptr state::get_timeperiod_by_id(unsigned int id) const {
  return (_timeperiod_by_id.value(id));
}

/**
 *  Get a contact by its id.
 *
 *  @param[in] id   The id of the contact.
 *
 *  @return         A contact::ptr to the contact, or a null contact::ptr.
 */
objects::contact::ptr state::get_contact_by_id(unsigned int id) const {
  return (_contacts.value(id));
}

/**
 *  Get a command by its id.
 *
 *  @param[în] id  The id of the command.
 *
 *  @return         A command::ptr to the command, or a null command::ptr.
 */
objects::command::ptr state::get_command_by_id(unsigned int id) const {
  return (_commands.value(id));
}

/**
 *  Get a read lock on this state.
 *
 *  @return  A QReadLocker locking this state.
 */
std::auto_ptr<QReadLocker> state::read_lock() {
  return (std::auto_ptr<QReadLocker>(new QReadLocker(&_state_mutex)));
}


/**
 *  Get a write lock on this state.
 *
 *  @return  A QWriteLocker locking this state.
 */
std::auto_ptr<QWriteLocker> state::write_lock() {
  return (std::auto_ptr<QWriteLocker>(new QWriteLocker(&_state_mutex)));

}

/**
 *  Is this node in downtime?
 *
 *  @param[in] id  The node id of the node.
 *
 *  @return        True of the node is in downtime.
 */
bool state::is_node_in_downtime(objects::node_id id) const {
  time_t current_time = time(NULL);
  QList<downtime::ptr> downtimes = _downtimes.values(id);

  for (QList<downtime::ptr>::const_iterator
         it(downtimes.begin()),
         end(downtimes.end());
       it != end;
       ++it) {
    if ((*it)->get_actual_end_time() > current_time &&
        (*it)->get_cancelled() == false &&
        (*it)->get_started() == true &&
        (*it)->get_actual_start_time() + (*it)->get_duration() > current_time)
      return (true);
  }

  return (false);
}

/**
 *  Has this node been acknowledged?
 *
 *  @param[in] id  The node id of the node.
 *
 *  @return        True of the node has been acknowledged.
 */
bool state::has_node_been_acknowledged(objects::node_id id) const {
  QList<acknowledgement::ptr> acknowledgements = _acks.values(id);

  for (QList<acknowledgement::ptr>::const_iterator
         it(acknowledgements.begin()),
         end(acknowledgements.end());
       it != end;
       ++it) {
    return (true);
  }
  return (false);
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
 *  Get a nodegroup by its name.
 *
 *  @param[in] name  The name of the nodegroup.
 *
 *  @return  The nodegroup or a null ptr.
 */
objects::nodegroup::ptr state::get_nodegroup_by_name(
                                 std::string const& name) const {
  return (_nodegroups_by_name.value(name));
}

/**
 *  Get the first contactgroup of a contact.
 *
 *  @param[in] name  The id of the contact..
 *
 *  @return  The contactgroup or a null ptr.
 */
objects::contactgroup::ptr state::get_contactgroup_by_contact_id(
                                    unsigned int contact_id) const {
  return (_contactgroups_by_contact_id.value(contact_id));
}

/**
 *  Get all the contacts in a contactgroup.
 *
 *  @param[in] cnt  The contactgroup.
 *
 *  @return  A list of the contacts.
 */
QList<unsigned int> state::get_contacts_by_contactgroup(
                             objects::contactgroup::ptr cnt) const {
  return (_contact_id_by_contactgroups.values(cnt));
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
    if (it.key().get_host_id() == id.get_host_id() &&
        it.key().is_service())
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
