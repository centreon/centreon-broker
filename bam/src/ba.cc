/*
** Copyright 2014-2015 Merethis
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

#include <sstream>
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi.hh"
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
    _last_kpi_update(0),
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
    timestamp last_state_change(impact->get_last_state_change());
    if (last_state_change.get_time_t() != (time_t)-1)
      _last_kpi_update = std::max(
                                _last_kpi_update.get_time_t(),
                                last_state_change.get_time_t());
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
           io::stream* visitor) {
  umap<kpi*, impact_info>::iterator
    it(_impacts.find(static_cast<kpi*>(child)));
  if (it != _impacts.end()) {
    // Get impact.
    impact_values new_hard_impact;
    impact_values new_soft_impact;
    it->second.kpi_ptr->impact_hard(new_hard_impact);
    it->second.kpi_ptr->impact_soft(new_soft_impact);

    // Logging.
    logging::debug(logging::low) << "BAM: BA " << _id
      << " is getting notified of child update (KPI "
      << it->second.kpi_ptr->get_id() << ", impact "
      << new_hard_impact.get_nominal() << ", last state change "
      << it->second.kpi_ptr->get_last_state_change() << ")";

    // If the new impact is the same as the old, don't update.
    if (it->second.hard_impact == new_hard_impact
        && it->second.soft_impact == new_soft_impact)
      return (false);
    timestamp last_state_change(
                it->second.kpi_ptr->get_last_state_change());
    if (last_state_change.get_time_t() != (time_t)-1)
      _last_kpi_update = std::max(
                                _last_kpi_update.get_time_t(),
                                last_state_change.get_time_t());

    // Discard old data.
    _unapply_impact(it->second);

    // Apply new data.
    it->second.hard_impact = new_hard_impact;
    it->second.soft_impact = new_soft_impact;
    _apply_impact(it->second);

    // Generate status event.
    visit(visitor);
  }
  return (true);
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
 *  Get the time at which the most recent KPI was updated.
 *
 *  @return Time at which the most recent KPI was updated.
 */
timestamp ba::get_last_kpi_update() const {
  return (_last_kpi_update);
}

/**
 *  Get the BA name.
 *
 *  @return BA name.
 */
std::string const& ba::get_name() const {
  return (_name);
}

/**
 *  Get the output.
 *
 *  @return Service output.
 */
std::string ba::get_output() const {
  std::ostringstream oss;
  oss << "BA : " << _name << " - current_level = "
      << static_cast<int>(normalize(_level_hard)) << "%";
  return (oss.str());
}

/**
 *  Get the performance data.
 *
 *  @return Performance data.
 */
std::string ba::get_perfdata() const {
  std::ostringstream oss;
  oss << "BA_Level=" << static_cast<int>(normalize(_level_hard)) << "%;"
      << static_cast<int>(_level_warning) << ";"
      << static_cast<int>(_level_critical) << ";0;100 "
      << "BA_Downtime=" << static_cast<int>(normalize(_downtime_hard));
  return (oss.str());
}

/**
 *  Get BA hard state.
 *
 *  @return BA hard state.
 */
short ba::get_state_hard() {
  short state;
  if (_level_hard <= _level_critical)
    state = 2;
  else if (_level_hard <= _level_warning)
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
  if (_level_soft <= _level_critical)
    state = 2;
  else if (_level_soft <= _level_warning)
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
  if (_event.isNull()) {
    _event = misc::shared_ptr<ba_event>(new ba_event(event));
    _last_kpi_update = _event->start_time;
  }
}

/**
 *  Set the BA name.
 *
 *  @param[in] name  New BA name.
 */
void ba::set_name(std::string const& name) {
  _name = name;
  return ;
}

/**
 *  Visit BA.
 *
 *  @param[out] visitor  Visitor that will receive BA status and events.
 */
void ba::visit(io::stream* visitor) {
  if (visitor) {
    // If no event was cached, create one if necessary.
    short hard_state(get_state_hard());
    bool state_changed(false);
    if (_event.isNull()) {
      if ((_last_kpi_update.get_time_t() == (time_t)-1)
          || (_last_kpi_update.get_time_t() == (time_t)0))
        _last_kpi_update = time(NULL);
      _open_new_event(visitor, hard_state);
    }
    // If state changed, close event and open a new one.
    else if ((_in_downtime != _event->in_downtime)
             || (hard_state != _event->status)) {
      state_changed = true;
      _event->end_time = _last_kpi_update;
      visitor->write(_event.staticCast<io::data>());
      _event.clear();
      _open_new_event(visitor, hard_state);
    }

    // Generate status event.
    misc::shared_ptr<ba_status> status(new ba_status);
    status->ba_id = _id;
    status->in_downtime = _in_downtime;
    if (!_event.isNull())
      status->last_state_change = _event->start_time;
    else
      status->last_state_change = _last_kpi_update;
    status->level_acknowledgement = normalize(_acknowledgement_hard);
    status->level_downtime = normalize(_downtime_hard);
    status->level_nominal = normalize(_level_hard);
    status->state = hard_state;
    status->state_changed = state_changed;
    logging::debug(logging::low)
      << "BAM: generating status of BA " << status->ba_id << " (state "
      << status->state << ", in downtime " << status->in_downtime
      << ", level " << status->level_nominal << ")";
    visitor->write(status.staticCast<io::data>());
  }
  return ;
}

/**
 *  @brief The service associated to this BA was updated.
 *
 *  Used to watch for downtime.
 *
 *  @param status   Status of the service.
 *  @param visitor  Visitor that will receive events.
 */
void ba::service_update(
          misc::shared_ptr<neb::service_status> const& status,
          io::stream* visitor) {
  (void) visitor;
  if (status->host_id == _host_id
      && status->service_id == _service_id) {
    // Log message.
    logging::debug(logging::low)
      << "BAM: BA " << _id << " is getting notified of service ("
      << _host_id << ", " << _service_id << ") update";

    // Check if there was a change.
    bool in_downtime(status->scheduled_downtime_depth > 0);
    if (_in_downtime != in_downtime) {
      _in_downtime = in_downtime;

      // Generate status event.
      visit(visitor);

      // Propagate change.
      propagate_update(visitor);
    }
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
  _last_kpi_update = right._last_kpi_update;
  _level_critical = right._level_critical;
  _level_hard = right._level_hard;
  _level_soft = right._level_soft;
  _level_warning = right._level_warning;
  return ;
}

/**
 *  Open a new event for this BA.
 *
 *  @param[out] visitor             Visitor that will receive events.
 *  @param[in]  service_hard_state  Hard state of virtual BA service.
 */
void ba::_open_new_event(
           io::stream* visitor,
           short service_hard_state) {
  _event = new ba_event;
  _event->ba_id = _id;
  _event->first_level = _level_hard < 0 ? 0 : _level_hard;
  _event->in_downtime = _in_downtime;
  _event->status = service_hard_state;
  _event->start_time = _last_kpi_update;
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
  _level_hard = 100.0;
  _level_soft = 100.0;
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
  // Prevent derive of values.
  ++_recompute_count;
  if (_recompute_count >= _recompute_limit)
    _recompute();

  // Adjust values.
  _acknowledgement_hard -= impact.hard_impact.get_acknowledgement();
  _acknowledgement_soft -= impact.soft_impact.get_acknowledgement();
  _downtime_hard -= impact.hard_impact.get_downtime();
  _downtime_soft -= impact.soft_impact.get_downtime();
  _level_hard += impact.hard_impact.get_nominal();
  _level_soft += impact.soft_impact.get_nominal();

  return ;
}
