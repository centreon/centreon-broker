/*
** Copyright 2014-2015, 2021 Centreon
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

#include "com/centreon/broker/bam/kpi_service.hh"

#include <cassert>
#include <cstring>

#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_service::kpi_service(uint32_t kpi_id,
                         uint32_t ba_id,
                         uint32_t host_id,
                         uint32_t service_id)
    : kpi(kpi_id, ba_id),
      _host_id(host_id),
      _service_id(service_id),
      _acknowledged(false),
      _downtimed(false),
      _impacts{0.0},
      _last_check(0),
      _state_hard(kpi_service::state::state_ok),
      _state_soft(kpi_service::state::state_ok),
      _state_type(0) {
  assert(_host_id);
}

/**
 *  Unused callback.
 *
 *  @param[in]  child    Unused.
 *  @param[out] visitor  Unused.
 *
 *  @return              True.
 */
bool kpi_service::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return true;
}

/**
 *  Get host ID.
 *
 *  @return Host ID.
 */
uint32_t kpi_service::get_host_id() const {
  return _host_id;
}

/**
 *  Get the impact applied when service is CRITICAL.
 *
 *  @return The impact applied when service is CRITICAL.
 */
double kpi_service::get_impact_critical() const {
  return _impacts[2];
}

/**
 *  Get the impact applied when service is UNKNOWN.
 *
 *  @return The impact applied when service is UNKNOWN.
 */
double kpi_service::get_impact_unknown() const {
  return _impacts[3];
}

/**
 *  Get the impact applied when service is WARNING.
 *
 *  @return The impact applied when service is WARNING.
 */
double kpi_service::get_impact_warning() const {
  return _impacts[1];
}

/**
 *  Get the service ID.
 *
 *  @return Service ID.
 */
uint32_t kpi_service::get_service_id() const {
  return _service_id;
}

/**
 *  Get the hard state of the service.
 *
 *  @return Hard state of the service.
 */
kpi_service::state kpi_service::get_state_hard() const {
  return _state_hard;
}

/**
 *  Get the soft state of the service.
 *
 *  @return Soft state of the service.
 */
kpi_service::state kpi_service::get_state_soft() const {
  return _state_soft;
}

/**
 *  Get current state type.
 *
 *  @return State type.
 */
short kpi_service::get_state_type() const {
  return _state_type;
}

/**
 *  Compute impact implied by the hard service state.
 *
 *  @param[out] impact Impacts implied by the hard service state.
 */
void kpi_service::impact_hard(impact_values& impact) {
  _fill_impact(impact, _state_hard);
}

/**
 *  Compute impact implied by the soft service state.
 *
 *  @param[out] impact Impacts implied by the soft service state.
 */
void kpi_service::impact_soft(impact_values& impact) {
  _fill_impact(impact, _state_soft);
}

/**
 *  Check if service is in downtime.
 *
 *  @return True if the service is in downtime.
 */
bool kpi_service::in_downtime() const {
  return _downtimed;
}

/**
 *  Check if service is acknowledged.
 *
 *  @return True if the service is acknowledged.
 */
bool kpi_service::is_acknowledged() const {
  return _acknowledged;
}

/**
 *  Service got updated !
 *
 *  @param[in]  status   Service status.
 *  @param[out] visitor  Object that will receive events.
 */
void kpi_service::service_update(
    std::shared_ptr<neb::service_status> const& status,
    io::stream* visitor) {
  if (status && status->host_id == _host_id &&
      status->service_id == _service_id) {
    // Log message.
    log_v2::bam()->debug(
        "BAM: KPI {} is getting notified of service ({}, {}) update", _id,
        _host_id, _service_id);

    // Update information.
    if (status->last_check.is_null()) {
      if (_last_check.is_null())
        _last_check = status->last_update;
    } else {
      _last_check = status->last_check;
    }
    _output = status->output;
    _perfdata = status->perf_data;
    _state_hard = static_cast<kpi_service::state>(status->last_hard_state);
    _state_soft = static_cast<kpi_service::state>(status->current_state);
    _state_type = status->state_type;

    // Generate status event.
    visit(visitor);

    // Propagate change.
    propagate_update(visitor);
  }
}

/**
 *  Service got an acknowledgement.
 *
 *  @param[in]  ack      Acknowledgement.
 *  @param[out] visitor  Object that will receive events.
 */
void kpi_service::service_update(
    std::shared_ptr<neb::acknowledgement> const& ack,
    io::stream* visitor) {
  if (ack && ack->host_id == _host_id && ack->service_id == _service_id) {
    // Log message.
    log_v2::bam()->debug(
        "BAM: KPI {} is getting an acknowledgement event for service ({}, {}) "
        "entry_time {} ; deletion_time {}",
        _id, _host_id, _service_id, ack->entry_time, ack->deletion_time);

    // Update information.
    _acknowledged = ack->deletion_time.is_null();

    // Generate status event.
    visit(visitor);

    // Propagate change.
    propagate_update(visitor);
  }
}

/**
 *  Service got a downtime.
 *
 *  @param[in]  dt
 *  @param[out] visitor  Object that will receive events.
 */
void kpi_service::service_update(std::shared_ptr<neb::downtime> const& dt,
                                 io::stream* visitor) {
  if (dt && dt->host_id == _host_id && dt->service_id == _service_id) {
    // Update information.
    _downtimed = dt->was_started && dt->actual_end_time.is_null();
    if (!_event || _event->in_downtime != _downtimed)
      _last_check = _downtimed ? dt->actual_start_time : dt->actual_end_time;

    // Log message.
    log_v2::bam()->debug(
        "BAM: KPI {} is getting a downtime event for service ({}, {}), in "
        "downtime: {} at {}",
        _id, _host_id, _service_id, _downtimed, _last_check);

    // Generate status event.
    visit(visitor);

    // Propagate change.
    propagate_update(visitor);
  }
}

/**
 *  Set service as acknowledged.
 *
 *  @param[in] acknowledged Acknowledged flag.
 */
void kpi_service::set_acknowledged(bool acknowledged) {
  _acknowledged = acknowledged;
}

/**
 *  Set service as downtimed.
 *
 *  @param[in] downtimed Downtimed flag.
 */
void kpi_service::set_downtimed(bool downtimed) {
  _downtimed = downtimed;
}

/**
 *  Set impact implied when service is CRITICAL.
 *
 *  @param[in] impact Impact if service is CRITICAL.
 */
void kpi_service::set_impact_critical(double impact) {
  _impacts[2] = impact;
}

/**
 *  Set impact implied when service is UNKNOWN.
 *
 *  @param[in] impact Impact if service is UNKNOWN.
 */
void kpi_service::set_impact_unknown(double impact) {
  _impacts[3] = impact;
}

/**
 *  Set impact implied when service is WARNING.
 *
 *  @param[in] impact Impact if service is WARNING.
 */
void kpi_service::set_impact_warning(double impact) {
  _impacts[1] = impact;
}

/**
 *  Set hard state.
 *
 *  @param[in] state Service hard state.
 */
void kpi_service::set_state_hard(kpi_service::state state) {
  _state_hard = state;
}

/**
 *  Set soft state.
 *
 *  @param[in] state Service soft state.
 */
void kpi_service::set_state_soft(kpi_service::state state) {
  _state_soft = state;
}

/**
 *  Set state type.
 *
 *  @param[in] type Current state type.
 */
void kpi_service::set_state_type(short type) {
  _state_type = type;
}

/**
 *  Visit service KPI.
 *
 *  @param[out] visitor  Object that will receive status.
 */
void kpi_service::visit(io::stream* visitor) {
  if (visitor) {
    // Commit the initial events saved in the cache.
    commit_initial_events(visitor);

    // Get information.
    impact_values hard_values;
    impact_values soft_values;
    impact_hard(hard_values);
    impact_soft(soft_values);

    // Generate BI events.
    {
      // If no event was cached, create one.
      if (!_event) {
        if (!_last_check.is_null()) {
          _open_new_event(visitor, hard_values);
        }
      }
      // If state changed, close event and open a new one.
      else if (_last_check >= _event->start_time &&
               (_downtimed != _event->in_downtime ||
                _state_hard != _event->status)) {
        _event->end_time = _last_check;
        visitor->write(std::static_pointer_cast<io::data>(_event));
        _open_new_event(visitor, hard_values);
      }
    }

    // Generate status event.
    {
      log_v2::bam()->debug("Generating kpi status {} for service", _id);
      std::shared_ptr<kpi_status> status{std::make_shared<kpi_status>(_id)};
      status->in_downtime = in_downtime();
      status->level_acknowledgement_hard = hard_values.get_acknowledgement();
      status->level_acknowledgement_soft = soft_values.get_acknowledgement();
      status->level_downtime_hard = hard_values.get_downtime();
      status->level_downtime_soft = soft_values.get_downtime();
      status->level_nominal_hard = hard_values.get_nominal();
      status->level_nominal_soft = soft_values.get_nominal();
      status->state_hard = _state_hard;
      status->state_soft = _state_soft;
      status->last_state_change = get_last_state_change();
      status->last_impact =
          _downtimed ? hard_values.get_downtime() : hard_values.get_nominal();
      log_v2::bam()->trace(
          "Writing kpi status {}: in downtime: {} ; last state changed: {}",
          _id, status->in_downtime, status->last_state_change);
      visitor->write(status);
    }
  }
}

/**
 *  Fill impact values from a state.
 *
 *  @param[out] impact Impacts of the state.
 *  @param[in]  state  Service state.
 */
void kpi_service::_fill_impact(impact_values& impact,
                               kpi_service::state state) {
  if (state < 0 || static_cast<size_t>(state) >= _impacts.size())
    throw msg_fmt("BAM: could not get impact introduced by state {}", state);
  double nominal{_impacts[state]};
  impact.set_nominal(nominal);
  impact.set_acknowledgement(_acknowledged ? nominal : 0.0);
  impact.set_downtime(_downtimed ? nominal : 0.0);
  impact.set_state(state);
}

/**
 *  Open a new event for this KPI.
 *
 *  @param[out] visitor  Visitor that will receive events.
 *  @param[in]  impacts  Impact values.
 */
void kpi_service::_open_new_event(io::stream* visitor,
                                  impact_values const& impacts) {
  _event = std::make_shared<kpi_event>(_id, _ba_id);
  _event->impact_level =
      _downtimed ? impacts.get_downtime() : impacts.get_nominal();
  _event->in_downtime = _downtimed;
  _event->output = _output;
  _event->perfdata = _perfdata;
  _event->start_time = _last_check;
  _event->status = _state_hard;
  log_v2::bam()->trace(
      "BAM: New BI event for kpi {}, ba {}, in downtime {} since {}", _id,
      _ba_id, _downtimed, _last_check);
  if (visitor) {
    /* We make a real copy because the writing into the DB is asynchronous and
     * so the event could have changed... */
    std::shared_ptr<io::data> ke{std::make_shared<kpi_event>(*_event)};
    visitor->write(ke);
  }
}

/**
 *  Set the initial event.
 *
 *  @param[in] e  the event.
 */
void kpi_service::set_initial_event(kpi_event const& e) {
  kpi::set_initial_event(e);
  _last_check = _event->start_time;
  _downtimed = _event->in_downtime;
}

/**
 *  Is this KPI in an ok state?
 *
 *  @return  True if this KPI is in an ok state.
 */
bool kpi_service::ok_state() const {
  return _state_hard == 0;
}
