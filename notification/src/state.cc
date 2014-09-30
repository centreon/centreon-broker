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

#include "com/centreon/broker/notification/state.hh"
#include "com/centreon/broker/notification/utilities/data_loggers.hh"

#include "com/centreon/broker/notification/builders/composed_acknowledgement_builder.hh"
#include "com/centreon/broker/notification/builders/composed_command_builder.hh"
#include "com/centreon/broker/notification/builders/composed_contact_builder.hh"
#include "com/centreon/broker/notification/builders/composed_dependency_builder.hh"
#include "com/centreon/broker/notification/builders/composed_downtime_builder.hh"
#include "com/centreon/broker/notification/builders/composed_escalation_builder.hh"
#include "com/centreon/broker/notification/builders/composed_node_builder.hh"
#include "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"

#include "com/centreon/broker/notification/builders/acknowledgement_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/command_by_name_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_command_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_name_builder.hh"
#include "com/centreon/broker/notification/builders/contact_by_node_builder.hh"
#include "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/escalation_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/node_set_builder.hh"
#include "com/centreon/broker/notification/builders/node_by_node_id_builder.hh"
#include "com/centreon/broker/notification/builders/timeperiod_by_name_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
state::state() :
  _state_mutex(QMutex::Recursive) {}

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
    _contact_by_name = obj._contact_by_name;
    _contacts = obj._contacts;
    _contact_by_command = obj._contact_by_command;
    _dependency_by_child_id = obj._dependency_by_child_id;
    _dependency_by_parent_id = obj._dependency_by_parent_id;
    _downtimes = obj._downtimes;
    _escalations = obj._escalations;
    _timeperiod_by_name = obj._timeperiod_by_name;
  }
  return (*this);
}

/**
 *  Get the objects from the db.
 */
void state::update_objects_from_db(QSqlDatabase& centreon_db,
                                   QSqlDatabase& centreon_storage_db) {
  // Acquire mutex.
  QMutexLocker lock(&_state_mutex);

  // Remove old objects.
  _nodes.clear();
  _node_by_id.clear();
  _acks.clear();
  _commands.clear();
  _contact_by_name.clear();
  _contacts.clear();
  _contact_by_command.clear();
  _dependency_by_child_id.clear();
  _dependency_by_parent_id.clear();
  _downtimes.clear();
  _escalations.clear();
  _timeperiod_by_name.clear();

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
    contact_by_command_builder by_command_builder(_contact_by_command);
    contact_by_name_builder by_name_builder(_contact_by_name);
    contact_by_node_builder by_node_builder(_contacts);
    composed.push_back(by_command_builder);
    composed.push_back(by_name_builder);
    composed.push_back(by_node_builder);
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
    // Get escalations.
    escalation_loader escalation;
    composed_escalation_builder composed;
    escalation_by_node_id_builder by_node_builder(_escalations);
    composed.push_back(by_node_builder);
    escalation.load(&centreon_db, &composed);
  }
  {
    // Get timeperiods.
    timeperiod_loader timeperiod;
    composed_timeperiod_builder composed;
    timeperiod_by_name_builder by_name_builder(_timeperiod_by_name);
    composed.push_back(by_name_builder);
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

  // Debug logging for all the data loaded.
#ifndef NDEBUG
    data_logger::log_container("_nodes", _nodes);
    data_logger::log_container("_node_by_id", _node_by_id);
    data_logger::log_container("_acks", _acks);
    data_logger::log_container("_commands", _commands);
    data_logger::log_container("_contact_by_command", _contact_by_name);
    data_logger::log_container("_contact_by_name", _contact_by_name);
    data_logger::log_container("_contacts", _contacts);
    data_logger::log_container("_dependency_by_child_id",
                               _dependency_by_child_id);
    data_logger::log_container("_dependency_by_parent_id",
                               _dependency_by_parent_id);
    data_logger::log_container("_downtimes", _downtimes);
    data_logger::log_container("_escalations", _escalations);
    data_logger::log_container("_timeperiod_by_name", _timeperiod_by_name);
#endif //!NDEBUG
}

node::ptr state::get_node_by_id(node_id id) {
  return (_node_by_id[id]);
}

/**
 *  Lock the internal mutex of this state.
 *
 *  @return  A QMutexLocker locking the internal mutex of this state.
 */
std::auto_ptr<QMutexLocker> state::lock() {
  std::auto_ptr<QMutexLocker> ret(new QMutexLocker(&_state_mutex));
  return (ret);
}
