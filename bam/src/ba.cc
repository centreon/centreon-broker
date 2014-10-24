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

#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi.hh"
#include "com/centreon/broker/bam/monitoring_stream.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Normalize the value of a double to be in the range [0,100].
 *
 *  @param[in] d  Value.
 *
 *  @return The provided value if in the range [0,100], the closest
 *          limit otherwise.
 */
static double normalize(double d) {
  if (d > 100.0)
    d = 100.0;
  else if (d < 0.0)
    d = 0.0;
  return (d);
}

/**
 *  Default constructor.
 */
ba::ba()
  : _acknowledgement_hard(0.0),
    _acknowledgement_soft(0.0),
    _downtime_hard(0.0),
    _downtime_soft(0.0),
    _host_id(0),
    _id(0),
    _in_downtime(false),
    _level_critical(0.0),
    _level_hard(100.0),
    _level_soft(100.0),
    _level_warning(0.0),
    _recompute_count(0),
    _service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
ba::ba(ba const& right) : computable(right), service_listener(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
ba::~ba() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
ba& ba::operator=(ba const& right) {
  if (this != &right) {
    computable::operator=(right);
    service_listener::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Add impact.
 *
 *  @param[in] impact KPI that will impact BA.
 */
void ba::add_impact(misc::shared_ptr<kpi> const& impact) {
  umap<kpi*, impact_info>::iterator
    it(_impacts.find(impact.data()));
  if (it == _impacts.end()) {
    impact_info& ii(_impacts[impact.data()]);
    ii.kpi_ptr = impact;
    impact->impact_hard(ii.hard_impact);
    impact->impact_soft(ii.soft_impact);
    _apply_impact(ii);
  }
  return ;
}

/**
 *  Notify BA of child update.
 *
 *  @param[in]  child    Child impact that got updated.
 *  @param[out] visitor  Object that will receive generated events.
 *
 *  @return True if the value of this ba was modified.
 */
bool ba::child_has_update(
           computable* child,
           monitoring_stream* visitor) {
  umap<kpi*, impact_info>::iterator
    it(_impacts.find(static_cast<kpi*>(child)));
  if (it != _impacts.end()) {
    // Logging.
    logging::debug(logging::low)
      << "BAM: BA " << _id << " is getting notified of child update";

    impact_values new_hard_impact;
    impact_values new_soft_impact;

    it->second.kpi_ptr->impact_hard(new_hard_impact);
    it->second.kpi_ptr->impact_soft(new_soft_impact);

    // If the new impact is the same as the old, don't update.
    if (it->second.hard_impact == new_hard_impact &&
        it->second.soft_impact == new_soft_impact)
      return false;

    // Discard old data.
    _unapply_impact(it->second);

    // Apply new data.
    it->second.hard_impact = new_hard_impact;
    it->second.soft_impact = new_soft_impact;
    _apply_impact(it->second);

    // Generate status event.
    visit(visitor);
  }
  return true;
}


/**
 *  Get the hard impact introduced by acknowledged KPI.
 *
 *  @return Hard impact introduced by acknowledged KPI.
 */
double ba::get_ack_impact_hard() {
  return (_acknowledgement_hard);
}

/**
 *  Get the soft impact introduced by acknowledged KPI.
 *
 *  @return Soft impact introduced by acknowledged KPI.
 */
double ba::get_ack_impact_soft() {
  return (_acknowledgement_soft);
}

/**
 *  Get the current BA event.
 *
 *  @return Current BA event, NULL if none is declared.
 */
ba_event* ba::get_ba_event() {
  return (_event.data());
}

/**
 *  Get the hard impact introduced by KPI in downtime.
 *
 *  @return Hard impact introduced by KPI in downtime.
 */
double ba::get_downtime_impact_hard() {
  return (_downtime_hard);
}

/**
 *  Get the soft impact introduced by KPI in downtime.
 *
 *  @return Soft impact introduced by KPI in downtime.
 */
double ba::get_downtime_impact_soft() {
  return (_downtime_soft);
}

/**
 *  Get the BA ID.
 *
 *  @return ID of this BA.
 */
unsigned int ba::get_id() {
  return (_id);
}

/**
 *  Get the id of the host associated to this ba.
 *
 *  @return  An integer representing the value of this id.
 */
unsigned int ba::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the id of the service associated to this ba.
 *
 *  @return  An integer representing the value of this id.
 */
unsigned int ba::get_service_id() const {
  return (_service_id);
}

/**
 *  @brief Check if the BA is in downtime.
 *
 *  The flag comes from the attached monitoring service.
 *
 *  @return True if the BA is in downtime, false otherwise.
 */
bool ba::get_in_downtime() const {
  return (_in_downtime);
}

/**
 *  Get the time of the last update of the attached service.
 *
 *  @return Time of the last update of the attached service.
 */
timestamp ba::get_last_service_update() const {
  return (_last_service_update);
}

/**
 *  Get the output.
 *
 *  @return Service output.
 */
std::string const& ba::get_output() const {
  return (_output);
}

/**
 *  Get the performance data.
 *
 *  @return Performance data.
 */
std::string const& ba::get_perfdata() const {
  return (_perfdata);
}

/**
 *  Get BA hard state.
 *
 *  @return BA hard state.
 */
short ba::get_state_hard() {
  short state;
  if (_level_hard < _level_critical)
    state = 2;
  else if (_level_hard < _level_warning)
    state = 1;
  else
    state = 0;
  return (state);
}

/**
 *  Get BA soft state.
 *
 *  @return BA soft state.
 */
short ba::get_state_soft() {
  short state;
  if (_level_soft < _level_critical)
    state = 2;
  else if (_level_soft < _level_warning)
    state = 1;
  else
    state = 0;
  return (state);
}

/**
 *  Remove child impact.
 *
 *  @param[in] impact Impact to remove.
 */
void ba::remove_impact(misc::shared_ptr<kpi> const& impact) {
  umap<kpi*, impact_info>::iterator
    it(_impacts.find(impact.data()));
  if (it != _impacts.end()) {
    _unapply_impact(it->second);
    _impacts.erase(it);
  }
  return ;
}

/**
 *  Set BA ID.
 *
 *  @param[in] id BA ID.
 */
void ba::set_id(unsigned int id) {
  _id = id;
  return ;
}

/**
 *  Set the service id associated to this ba.
 *
 *  @param[in] service_id  Set the service id.
 */
void ba::set_service_id(unsigned int service_id) {
  _service_id = service_id;
}

/**
 *  Set the host id associated to this ba.
 *
 *  @param[in] host_id  Set the service id.
 */
void ba::set_host_id(unsigned int host_id) {
  _host_id = host_id;
}

/**
 *  Set critical level.
 *
 *  @param[in] level  Critical level.
 */
void ba::set_level_critical(double level) {
  _level_critical = level;
  return ;
}

/**
 *  Set warning level.
 *
 *  @param[in] level  Warning level.
 */
void ba::set_level_warning(double level) {
  _level_warning = level;
  return ;
}

/**
 *  @brief Set the initial, opened event of this ba.
 *
 *  Useful for recovery after cbd stop.
 *
 *  @param[in] event  The event to set.
 */
void ba::set_initial_event(ba_event const& event) {
  if (_event.isNull())
    _event = misc::shared_ptr<ba_event>(new ba_event(event));
}

/**
 *  Add a timeperiod associated with this ba.
 *
 *  @param[in] tp          The timeperiod to add.
 *  @param[in] is_default  True if this timeperiod is the default timeperiod.
 */
void ba::add_timeperiod(time::timeperiod::ptr tp, bool is_default) {
  _tps.push_back(std::make_pair(tp, is_default));
}

/**
 *  Clear the timeperiods associated with this ba.
 */
void ba::clear_timeperiods() {
  _tps.clear();
}

/**
 *  Visit BA.
 *
 *  @param[out] visitor  Visitor that will receive BA status and events.
 */
void ba::visit(monitoring_stream* visitor) {
  if (visitor) {
    // Generate status event.
    {
      misc::shared_ptr<ba_status> status(new ba_status);
      status->ba_id = _id;
      status->level_acknowledgement = normalize(_acknowledgement_hard);
      status->level_downtime = normalize(_downtime_hard);
      status->level_nominal = normalize(_level_hard);
      visitor->write(status.staticCast<io::data>());
    }

    // Generate BI events.
    {
      // If no event was cached, create one.
      if (_event.isNull())
        _open_new_event(visitor);
      // If state changed, close event and open a new one.
      else if ((_in_downtime != _event->in_downtime)
               || (get_state_hard() != _event->status)) {
        _event->end_time = _last_service_update;
        visitor->write(_event.staticCast<io::data>());
        _event.clear();
        _open_new_event(visitor);
      }
    }
  }
  return ;
}

/**
 *  @brief The service associated to this ba was updated.
 *
 *  Used to watch for downtime and update output and perfdata.
 *
 *  @param status   Status of the service.
 *  @param visitor  Visitor that will receive events.
 */
void ba::service_update(
          misc::shared_ptr<neb::service_status> const& status,
          monitoring_stream* visitor) {
  (void) visitor;
  logging::debug(logging::low)
    << "BAM: BA " << _id << " is getting notified of service update";

  if (status->host_id == _host_id
      && status->service_id == _service_id) {
    // Set downtime.
    _in_downtime = (status->scheduled_downtime_depth > 0);
    // Set last service update.
    _last_service_update = status->last_update;
    // Set output.
    _output = status->output.toStdString();
    // Set perfdata.
    _perfdata = status->perf_data.toStdString();

    // Generate status event.
    visit(visitor);

    // Propagate change.
    propagate_update(visitor);
  }
  else
    logging::error(logging::medium)
      << "BAM: BA " << _id << " has got an invalid status service."
         " This should never happen : check your database. "
         "(BA host id = " << _host_id << ", service id = " << _service_id <<
         ", inbound host id = " << status->host_id
      << ", service_id = " << status->service_id << ").";
}

/**
 *  Apply some impact.
 *
 *  @param[in] impact Impact information.
 */
void ba::_apply_impact(ba::impact_info& impact) {
  // Adjust values.
  _acknowledgement_hard += impact.hard_impact.get_acknowledgement();
  _acknowledgement_soft += impact.soft_impact.get_acknowledgement();
  _downtime_hard += impact.hard_impact.get_downtime();
  _downtime_soft += impact.soft_impact.get_downtime();
  _level_hard -= impact.hard_impact.get_nominal();
  _level_soft -= impact.soft_impact.get_nominal();

  // Prevent derive of values.
  _recompute_count = _recompute_count >= 0 ? _recompute_count + 1 :
                                           _recompute_count;
  if (_recompute_count >= _recompute_limit)
    _recompute();

  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void ba::_internal_copy(ba const& right) {
  _acknowledgement_hard = right._acknowledgement_hard;
  _acknowledgement_soft = right._acknowledgement_soft;
  _downtime_hard = right._downtime_hard;
  _downtime_soft = right._downtime_soft;
  _event = right._event;
  _id = right._id;
  _service_id = right._service_id;
  _host_id = right._host_id;
  _impacts = right._impacts;
  _in_downtime = right._in_downtime;
  _last_service_update = right._last_service_update;
  _level_critical = right._level_critical;
  _level_hard = right._level_hard;
  _level_soft = right._level_soft;
  _level_warning = right._level_warning;
  _output = right._output;
  _perfdata = right._perfdata;
  _tps = right._tps;
  return ;
}

/**
 *  Open a new event for this BA.
 *
 *  @param[out] visitor  Visitor that will receive events.
 */
void ba::_open_new_event(monitoring_stream* visitor) {
  _event = new ba_event;
  _event->ba_id = _id;
  _event->in_downtime = _in_downtime;
  _event->status = get_state_hard();
  _event->start_time = _last_service_update;
  if (visitor) {
    misc::shared_ptr<io::data> be(new ba_event(*_event));
    visitor->write(be);
  }
  return ;
}

/**
 *  @brief Recompute all impacts.
 *
 *  This method was created to prevent the real values to derive to
 *  much from their true value due to the caching system.
 */
void ba::_recompute() {
  _acknowledgement_hard = 0.0;
  _acknowledgement_soft = 0.0;
  _downtime_hard = 0.0;
  _downtime_soft = 0.0;
  _level_hard = 0.0;
  _level_soft = 0.0;
  // Set _recompute_count to an invalid, guard value.
  _recompute_count = -1;
  for (umap<kpi*, impact_info>::iterator
         it(_impacts.begin()),
         end(_impacts.end());
       it != end;
       ++it)
    _apply_impact(it->second);
  _recompute_count = 0;
  return ;
}

/**
 *  Unapply some impact.
 *
 *  @param[in] impact Impact information.
 */
void ba::_unapply_impact(ba::impact_info& impact) {
  // Adjust values.
  _acknowledgement_hard -= impact.hard_impact.get_acknowledgement();
  _acknowledgement_soft -= impact.soft_impact.get_acknowledgement();
  _downtime_hard -= impact.hard_impact.get_downtime();
  _downtime_soft -= impact.soft_impact.get_downtime();
  _level_hard += impact.hard_impact.get_nominal();
  _level_soft += impact.soft_impact.get_nominal();

  // Prevent derive of values.
  ++_recompute_count;
  if (_recompute_count >= _recompute_limit)
    _recompute();

  return ;
}

/**
 *  @brief Compute and write the duration events associated with a ba event.
 *
 *  The event durations are computed from the associated timeperiods of this BA.
 *
 *  @param[in] ev       The ba_event generating the durations.
 *  @param[in] visitor  A visitor stream.
 */
void ba::_compute_event_durations(
           misc::shared_ptr<ba_event> ev,
           monitoring_stream* visitor) {
  if (ev.isNull() || !visitor)
    return ;

  for (std::vector<std::pair<time::timeperiod::ptr, bool> >::iterator
         it(_tps.begin()),
         end(_tps.end());
       it != end;
       ++it) {
    misc::shared_ptr<ba_duration_event> dur_ev(new ba_duration_event);
    dur_ev->ba_id = _id;
    dur_ev->real_start_time = ev->start_time;
    dur_ev->start_time = it->first->get_next_valid(ev->start_time);
    dur_ev->end_time = ev->end_time;
    dur_ev->duration = dur_ev->end_time - dur_ev->start_time;
    dur_ev->sla_duration = it->first->duration_intersect(dur_ev->start_time,
                                                         dur_ev->end_time);
    dur_ev->timeperiod_id = it->first->get_id();
    dur_ev->timeperiod_is_default = it->second;
    visitor->write(dur_ev);
  }
}
