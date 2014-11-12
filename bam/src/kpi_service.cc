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

#include <cstring>
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi_service.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_service::kpi_service()
  : _acknowledged(false),
    _downtimed(false),
    _host_id(0),
    _last_check(0),
    _service_id(0),
    _state_hard(0),
    _state_soft(0),
    _state_type(0) {
  for (unsigned int i(0); i < sizeof(_impacts) / sizeof(*_impacts); ++i)
    _impacts[i] = 0.0;
}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
kpi_service::kpi_service(kpi_service const& right)
  : service_listener(right), kpi(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
kpi_service::~kpi_service() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
kpi_service& kpi_service::operator=(kpi_service const& right) {
  if (this != &right) {
    service_listener::operator=(right);
    kpi::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Unused callback.
 *
 *  @param[in]  child    Unused.
 *  @param[out] visitor  Unused.
 *
 *  @return              True.
 */
bool kpi_service::child_has_update(
                    computable* child,
                    io::stream* visitor) {
  (void)child;
  (void)visitor;
  return true;
}

/**
 *  Get host ID.
 *
 *  @return Host ID.
 */
unsigned int kpi_service::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the impact applied when service is CRITICAL.
 *
 *  @return The impact applied when service is CRITICAL.
 */
double kpi_service::get_impact_critical() const {
  return (_impacts[2]);
}

/**
 *  Get the impact applied when service is UNKNOWN.
 *
 *  @return The impact applied when service is UNKNOWN.
 */
double kpi_service::get_impact_unknown() const {
  return (_impacts[3]);
}

/**
 *  Get the impact applied when service is WARNING.
 *
 *  @return The impact applied when service is WARNING.
 */
double kpi_service::get_impact_warning() const {
  return (_impacts[1]);
}

/**
 *  Get the service ID.
 *
 *  @return Service ID.
 */
unsigned int kpi_service::get_service_id() const {
  return (_service_id);
}

/**
 *  Get the hard state of the service.
 *
 *  @return Hard state of the service.
 */
short kpi_service::get_state_hard() const {
  return (_state_hard);
}

/**
 *  Get the soft state of the service.
 *
 *  @return Soft state of the service.
 */
short kpi_service::get_state_soft() const {
  return (_state_soft);
}

/**
 *  Get current state type.
 *
 *  @return State type.
 */
short kpi_service::get_state_type() const {
  return (_state_type);
}

/**
 *  Compute impact implied by the hard service state.
 *
 *  @param[out] impact Impacts implied by the hard service state.
 */
void kpi_service::impact_hard(impact_values& impact) {
  _fill_impact(impact, _state_hard);
  return ;
}

/**
 *  Compute impact implied by the soft service state.
 *
 *  @param[out] impact Impacts implied by the soft service state.
 */
void kpi_service::impact_soft(impact_values& impact) {
  _fill_impact(impact, _state_soft);
  return ;
}

/**
 *  Check if service is in downtime.
 *
 *  @return True if the service is in downtime.
 */
bool kpi_service::in_downtime() const {
  return (_downtimed);
}

/**
 *  Check if service is acknowledged.
 *
 *  @return True if the service is acknowledged.
 */
bool kpi_service::is_acknowledged() const {
  return (_acknowledged);
}

/**
 *  Service got updated !
 *
 *  @param[in]  status   Service status.
 *  @param[out] visitor  Object that will receive events.
 */
void kpi_service::service_update(
                    misc::shared_ptr<neb::service_status> const& status,
                    io::stream* visitor) {
  if (!status.isNull()
      && (status->host_id == _host_id)
      && (status->service_id == _service_id)) {
    // Log message.
    logging::debug(logging::low) << "BAM: KPI " << _id
      << " is getting notified of service (" << _host_id << ", "
      << _service_id << ") update";

    // Update information.
    _acknowledged = status->problem_has_been_acknowledged;
    _downtimed = status->scheduled_downtime_depth;
    if ((status->last_check == (time_t)-1)
        || (status->last_check == (time_t)0)) {
      if ((_last_check == (time_t)-1)
          || (_last_check == (time_t)0))
        _last_check = status->last_update;
    }
    else
      _last_check = status->last_check;
    _output = status->output.toStdString();
    _perfdata = status->perf_data.toStdString();
    _state_hard = status->last_hard_state;
    _state_soft = status->current_state;
    _state_type = status->state_type;

    // Generate status event.
    visit(visitor);

    // Propagate change.
    propagate_update(visitor);
  }
  return ;
}

/**
 *  Set service as acknowledged.
 *
 *  @param[in] acknowledged Acknowledged flag.
 */
void kpi_service::set_acknowledged(bool acknowledged) {
  _acknowledged = acknowledged;
  return ;
}

/**
 *  Set service as downtimed.
 *
 *  @param[in] downtimed Downtimed flag.
 */
void kpi_service::set_downtimed(bool downtimed) {
  _downtimed = downtimed;
  return ;
}

/**
 *  Set host ID.
 *
 *  @param[in] host_id Host ID.
 */
void kpi_service::set_host_id(unsigned int host_id) {
  _host_id = host_id;
  return ;
}

/**
 *  Set impact implied when service is CRITICAL.
 *
 *  @param[in] impact Impact if service is CRITICAL.
 */
void kpi_service::set_impact_critical(double impact) {
  _impacts[2] = impact;
  return ;
}

/**
 *  Set impact implied when service is UNKNOWN.
 *
 *  @param[in] impact Impact if service is UNKNOWN.
 */
void kpi_service::set_impact_unknown(double impact) {
  _impacts[3] = impact;
  return ;
}

/**
 *  Set impact implied when service is WARNING.
 *
 *  @param[in] impact Impact if service is WARNING.
 */
void kpi_service::set_impact_warning(double impact) {
  _impacts[1] = impact;
  return ;
}

/**
 *  Set service ID.
 *
 *  @param[in] service_id Service ID.
 */
void kpi_service::set_service_id(unsigned int service_id) {
  _service_id = service_id;
  return ;
}

/**
 *  Set hard state.
 *
 *  @param[in] state Service hard state.
 */
void kpi_service::set_state_hard(short state) {
  _state_hard = state;
  return ;
}

/**
 *  Set soft state.
 *
 *  @param[in] state Service soft state.
 */
void kpi_service::set_state_soft(short state) {
  _state_soft = state;
  return ;
}

/**
 *  Set state type.
 *
 *  @param[in] type Current state type.
 */
void kpi_service::set_state_type(short type) {
  _state_type = type;
  return ;
}

/**
 *  Visit service KPI.
 *
 *  @param[out] visitor  Object that will receive status.
 */
void kpi_service::visit(io::stream* visitor) {
  if (visitor) {
    // Get information.
    impact_values hard_values;
    impact_values soft_values;
    impact_hard(hard_values);
    impact_soft(soft_values);

    // Generate status event.
    {
      misc::shared_ptr<kpi_status> status(new kpi_status);
      status->kpi_id = _id;
      status->level_acknowledgement_hard = hard_values.get_acknowledgement();
      status->level_acknowledgement_soft = soft_values.get_acknowledgement();
      status->level_downtime_hard = hard_values.get_downtime();
      status->level_downtime_soft = soft_values.get_downtime();
      status->level_nominal_hard = hard_values.get_nominal();
      status->level_nominal_soft = soft_values.get_nominal();
      status->state_hard = _state_hard;
      status->state_soft = _state_soft;
      visitor->write(status.staticCast<io::data>());
    }

    // Generate BI events.
    {
      // If no event was cached, create one.
      if (_event.isNull()) {
        if ((_last_check.get_time_t() != (time_t)-1)
            && (_last_check.get_time_t() != (time_t)0))
        _open_new_event(visitor, hard_values);
      }
      // If state changed, close event and open a new one.
      else if ((_last_check > _event->start_time)
               && ((_downtimed != _event->in_downtime)
                   || (_state_hard != _event->status))) {
        _event->end_time = _last_check;
        visitor->write(_event.staticCast<io::data>());
        _event.clear();
        _open_new_event(visitor, hard_values);
      }
    }
  }
}

/**
 *  Fill impact values from a state.
 *
 *  @param[out] impact Impacts of the state.
 *  @param[in]  state  Service state.
 */
void kpi_service::_fill_impact(impact_values& impact, short state) {
  if ((state < 0)
      || (static_cast<size_t>(state)
          >= (sizeof(_impacts) / sizeof(*_impacts))))
    throw (exceptions::msg()
           << "BAM: could not get impact introduced by state "
           << state);
  double nominal(_impacts[state]);
  impact.set_nominal(nominal);
  impact.set_acknowledgement(_acknowledged ? nominal : 0.0);
  impact.set_downtime(_downtimed ? nominal : 0.0);
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void kpi_service::_internal_copy(kpi_service const& right) {
  _acknowledged = right._acknowledged;
  _downtimed = right._downtimed;
  _event = right._event;
  _host_id = right._host_id;
  memcpy(_impacts, right._impacts, sizeof(_impacts));
  _last_check = right._last_check;
  _output = right._output;
  _perfdata = right._perfdata;
  _service_id = right._service_id;
  _state_hard = right._state_hard;
  _state_soft = right._state_soft;
  _state_type = right._state_type;
  return ;
}

/**
 *  Open a new event for this KPI.
 *
 *  @param[out] visitor  Visitor that will receive events.
 *  @param[in]  impacts  Impact values.
 */
void kpi_service::_open_new_event(
                    io::stream* visitor,
                    impact_values const& impacts) {
  _event = new kpi_event;
  _event->kpi_id = _id;
  _event->impact_level = _event->in_downtime
                         ? impacts.get_downtime()
                         : impacts.get_nominal();
  _event->in_downtime = _downtimed;
  _event->output = _output.c_str();
  _event->perfdata = _perfdata.c_str();
  _event->start_time = _last_check;
  _event->status = _state_hard;
  if (visitor) {
    misc::shared_ptr<io::data> ke(new kpi_event(*_event));
    visitor->write(ke);
  }
  return ;
}
