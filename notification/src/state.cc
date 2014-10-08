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
#include "com/centreon/broker/notification/builders/command_by_name_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/node_set_builder.hh"
#include "com/centreon/broker/notification/builders/node_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/notification_method_by_id_builder.hh"
#include "com/centreon/broker/notification/builders/notification_rule_by_node_builder.hh"

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
  }
  return (*this);
}

/**
 *  Get the objects from the db.
 */
void state::update_objects_from_db(QSqlDatabase& centreon_db,
                                   QSqlDatabase& centreon_storage_db) {
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
    command_by_name_builder by_name_builder(_commands);
    composed.push_back(by_name_builder);
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
  {
    // Get downtimes.
    downtime_loader downtime;
    composed_downtime_builder composed;
    downtime_by_node_id_builder by_node_builder(_downtimes);
    composed.push_back(by_node_builder);
    downtime.load(&centreon_storage_db, &composed);
  }
  {
    // Get acknowledgements.
    acknowledgement_loader ack;
    composed_acknowledgement_builder composed;
    acknowledgement_by_node_id_builder by_node_builder(_acks);
    composed.push_back(by_node_builder);
    ack.load(&centreon_storage_db, &composed);
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
    notification_rule_by_node_builder by_node_builder(_notification_rules_by_node);
    composed.push_back(by_node_builder);
    nrl.load(&centreon_db, &composed);
  }

  // Debug logging for all the data loaded.
#ifndef NDEBUG
    data_logger::log_container("_nodes", _nodes);
    data_logger::log_container("_node_by_id", _node_by_id);
    data_logger::log_container("_acks", _acks);
    data_logger::log_container("_commands", _commands);
    data_logger::log_container("_contacts", _contacts);
    data_logger::log_container("_dependency_by_child_id",
                               _dependency_by_child_id);
    data_logger::log_container("_dependency_by_parent_id",
                               _dependency_by_parent_id);
    data_logger::log_container("_downtimes", _downtimes);
    data_logger::log_container("_timeperiod_by_id", _timeperiod_by_id);
    /*data_logger::log_container("_notification_methods", _notification_methods);
    data_logger::log_container("_notification_rules_by_node", _notification_rules_by_node);*/
#endif //!NDEBUG
}

/**
 *  Get a node by its node id.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return        A node::ptr to the node, or a null node::ptr.
 */
node::ptr state::get_node_by_id(node_id id) {
  return (_node_by_id.value(id));
}

/**
 *  Get the host of a service.
 *
 *  @param[in] service_id  The node id of the service.
 *
 *  @return                A node::ptr to the host, or a null node::ptr.
 */
node::ptr state::get_host_from_service(objects::node_id service_id) {
  if (!service_id.is_service())
    return (node::ptr());
  else
    return (_node_by_id.value(node_id(service_id.get_host_id())));
}

/**
 * @brief state::get_timeperiod_by_node
 * @return
 */
objects::timeperiod::ptr state::get_timeperiod_by_node(objects::node_id id) {
  return (timeperiod::ptr());
}

/**
 *  Get a timeperiod by its id.
 *
 *  @param[in] id    The id of the timeperiod.
 *
 *  @return          A timeperiod::ptr to the timeperiod, or a null timeperiod::ptr.
 */
timeperiod::ptr state::get_timeperiod_by_id(unsigned int id) {
  return (_timeperiod_by_id.value(id));
}

/**
 *  Get the contacts associated with a node.
 *
 *  @param[in] id  The node id of the node.
 *
 *  @return        A list of contact::ptr to the contacts of this node.
 */
QList<contact::ptr> state::get_contacts_by_node(objects::node_id id) {
  return (QList<contact::ptr>());
  //return (_contacts.values(id));
}

/**
 *  Get the host commands of a contact.
 *
 *  @param[in] cnt  The contact.
 *
 *  @return         A list of command::ptr to the host commands of this contact.
 */
QList<command::ptr> state::get_host_commands_by_contact(contact::ptr cnt) {
  return (QList<command::ptr>());
}

/**
 *  Get the service commands of a contact.
 *
 *  @param[in] cnt  The contact.
 *
 *  @return         A list of command::ptr to the service commands of this contact.
 */
QList<command::ptr> state::get_service_commands_by_contact(contact::ptr cnt) {
  return (QList<command::ptr>());
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
bool state::is_node_in_downtime(objects::node_id id) {
  time_t current_time = time(NULL);
  QList<downtime::ptr> downtimes = _downtimes.values(id);

  for (QList<downtime::ptr>::iterator it(downtimes.begin()),
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
bool state::has_node_been_acknowledged(objects::node_id id) {
  time_t current_time = time(NULL);
  QList<acknowledgement::ptr> acknowledgements = _acks.values(id);

  for (QList<acknowledgement::ptr>::iterator it(acknowledgements.begin()),
                                             end(acknowledgements.end());
       it != end;
       ++it) {
    return (true);
  }
  return (false);
}
