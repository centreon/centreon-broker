/*
** Copyright 2014 Centreon
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
      _state_hard(0),
      _state_soft(0),
      _state_known(false),
      _in_downtime(false) {}

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
 *
 *  @return             True.
 */
bool bool_service::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return true;
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
 *  Set host ID.
 *
 *  @param[in] host_id Host ID.
 */
void bool_service::set_host_id(unsigned int host_id) {
  _host_id = host_id;
  return;
}

/**
 *  Set service ID.
 *
 *  @param[in] service_id Service ID.
 */
void bool_service::set_service_id(unsigned int service_id) {
  _service_id = service_id;
  return;
}

/**
 *  Notify of service update.
 *
 *  @param[in]  status   Service status.
 *  @param[out] visitor  Object that will receive events.
 */
void bool_service::service_update(
    std::shared_ptr<neb::service_status> const& status,
    io::stream* visitor) {
  if (status && status->host_id == _host_id &&
      status->service_id == _service_id) {
    _state_hard = status->last_hard_state;
    _state_soft = status->current_state;
    _state_known = true;
    _in_downtime = (status->downtime_depth > 0);
    propagate_update(visitor);
  }
}

/**
 *  Get the hard value.
 *
 *  @return Hard value.
 */
double bool_service::value_hard() {
  return (_state_hard);
}

/**
 *  Get the soft value.
 *
 *  @preturn Soft value.
 */
double bool_service::value_soft() {
  return (_state_soft);
}

/**
 *  Get if the state is known, i.e has been computed at least once.
 *
 *  @return  True if the state is known.
 */
bool bool_service::state_known() const {
  return (_state_known);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void bool_service::_internal_copy(bool_service const& right) {
  _host_id = right._host_id;
  _service_id = right._service_id;
  _state_hard = right._state_hard;
  _state_soft = right._state_soft;
  _state_known = right._state_known;
  _in_downtime = right._in_downtime;
  return;
}

/**
 *  Get is this boolean service is in downtime.
 *
 *  @return  True if in downtime.
 */
bool bool_service::in_downtime() const {
  return (_in_downtime);
}
