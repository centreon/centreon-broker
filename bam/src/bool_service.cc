/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/bool_service.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_service::bool_service()
  : _host_id(0),
    _service_id(0),
    _state_expected(0),
    _state_hard(0),
    _state_soft(0),
    _value_if_state_match(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_service::bool_service(bool_service const& right)
  : bool_value(right), service_listener(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
bool_service::~bool_service() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_service& bool_service::operator=(bool_service const& right) {
  if (this != &right) {
    bool_value::operator=(right);
    service_listener::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  @brief Notification of child update.
 *
 *  This method is unused by bool_service because it has no computable
 *  child that could influence its value.
 *
 *  @param[in]  child   Unused.
 *  @param[out] visitor Unused.
 */
void bool_service::child_has_update(computable* child, stream* visitor) {
  (void)child;
  (void)visitor;
  return ;
}

/**
 *  Get the host ID.
 *
 *  @return Host ID.
 */
unsigned int bool_service::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the service ID.
 *
 *  @return Service ID.
 */
unsigned int bool_service::get_service_id() const {
  return (_service_id);
}

/**
 *  Set the expected state of the service.
 *
 *  @param[in] expected Expected state of the service.
 */
void bool_service::set_expected_state(short expected) {
  _state_expected = expected;
  return ;
}

/**
 *  Set host ID.
 *
 *  @param[in] host_id Host ID.
 */
void bool_service::set_host_id(unsigned int host_id) {
  _host_id = host_id;
  return ;
}

/**
 *  Set service ID.
 *
 *  @param[in] service_id Service ID.
 */
void bool_service::set_service_id(unsigned int service_id) {
  _service_id = service_id;
  return ;
}

/**
 *  Set value if service state match the expected state.
 *
 *  @param[in] value Value to return with value_hard() and value_soft()
 *                   if the service state match the expected state.
 */
void bool_service::set_value_if_state_match(bool value) {
  _value_if_state_match = value;
  return ;
}

/**
 *  Notify of service update.
 *
 *  @param[in]  status   Service status.
 *  @param[out] visitor  Object that will receive events.
 */
void bool_service::service_update(
                     misc::shared_ptr<neb::service_status> const& status,
                     stream* visitor) {
  if (!status.isNull()
      && (status->host_id == _host_id)
      && (status->service_id == _service_id)) {
    _state_hard = status->last_hard_state;
    _state_soft = status->current_state;
    propagate_update(visitor);
  }
  return ;
}

/**
 *  Get the hard value.
 *
 *  @return Hard value.
 */
bool bool_service::value_hard() {
  return ((_state_hard == _state_expected)
          ? _value_if_state_match
          : !_value_if_state_match);
}

/**
 *  Get the soft value.
 *
 *  @preturn Soft value.
 */
bool bool_service::value_soft() {
  return ((_state_soft == _state_expected)
          ? _value_if_state_match
          : !_value_if_state_match);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void bool_service::_internal_copy(bool_service const& right) {
  _host_id = right._host_id;
  _service_id = right._service_id;
  _state_expected = right._state_expected;
  _state_hard = right._state_hard;
  _state_soft = right._state_soft;
  _value_if_state_match = right._value_if_state_match;
  return ;
}
