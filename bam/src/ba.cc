/*
** Copyright 2014-2016 Centreon
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

#include "com/centreon/broker/bam/ba.hh"
#include <sstream>
#include "com/centreon/broker/bam/ba_status.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/service_status.hh"

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
 *  Constructor.
 *
 *  @param[in] generate_virtual_status  Whether or not the BA object
 *                                      should generate statuses of
 *                                      virtual hosts and services.
 */
ba::ba(bool generate_virtual_status)
    : _acknowledgement_hard(0.0),
      _acknowledgement_soft(0.0),
      _downtime_hard(0.0),
      _downtime_soft(0.0),
      _generate_virtual_status(generate_virtual_status),
      _host_id(0),
      _id(0),
      _in_downtime(false),
      _last_kpi_update(0),
      _level_critical(0.0),
      _level_hard(100.0),
      _level_soft(100.0),
      _level_warning(0.0),
      _recompute_count(0),
      _service_id(0),
      _valid(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
ba::ba(ba const& other) : computable(other), service_listener(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
ba::~ba() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
ba& ba::operator=(ba const& other) {
  if (this != &other) {
    computable::operator=(other);
    service_listener::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Add impact.
 *
 *  @param[in] impact KPI that will impact BA.
 */
void ba::add_impact(std::shared_ptr<kpi> const& impact) {
  std::unordered_map<kpi*, impact_info>::iterator it(
      _impacts.find(impact.get()));
  if (it == _impacts.end()) {
    impact_info& ii(_impacts[impact.get()]);
    ii.kpi_ptr = impact;
    impact->impact_hard(ii.hard_impact);
    impact->impact_soft(ii.soft_impact);
    ii.in_downtime = impact->in_downtime();
    _apply_impact(ii);
    timestamp last_state_change(impact->get_last_state_change());
    if (last_state_change.get_time_t() != (time_t)-1)
      _last_kpi_update = std::max(_last_kpi_update.get_time_t(),
                                  last_state_change.get_time_t());
  }
  return;
}

/**
 *  Notify BA of child update.
 *
 *  @param[in]  child    Child impact that got updated.
 *  @param[out] visitor  Object that will receive generated events.
 *
 *  @return True if the value of this ba was modified.
 */
bool ba::child_has_update(computable* child, io::stream* visitor) {
  std::unordered_map<kpi*, impact_info>::iterator it(
      _impacts.find(static_cast<kpi*>(child)));
  if (it != _impacts.end()) {
    // Get impact.
    impact_values new_hard_impact;
    impact_values new_soft_impact;
    it->second.kpi_ptr->impact_hard(new_hard_impact);
    it->second.kpi_ptr->impact_soft(new_soft_impact);
    bool kpi_in_downtime(it->second.kpi_ptr->in_downtime());

    // Logging.
    logging::debug(logging::low)
        << "BAM: BA " << _id << " is getting notified of child update (KPI "
        << it->second.kpi_ptr->get_id() << ", impact "
        << new_hard_impact.get_nominal() << ", last state change "
        << it->second.kpi_ptr->get_last_state_change() << ")";

    // If the new impact is the same as the old, don't update.
    if (it->second.hard_impact == new_hard_impact &&
        it->second.soft_impact == new_soft_impact &&
        it->second.in_downtime == kpi_in_downtime)
      return (false);
    timestamp last_state_change(it->second.kpi_ptr->get_last_state_change());
    if (last_state_change.get_time_t() != (time_t)-1)
      _last_kpi_update = std::max(_last_kpi_update.get_time_t(),
                                  last_state_change.get_time_t());

    // Discard old data.
    _unapply_impact(it->second);

    // Apply new data.
    it->second.hard_impact = new_hard_impact;
    it->second.soft_impact = new_soft_impact;
    it->second.in_downtime = kpi_in_downtime;
    _apply_impact(it->second);

    // Check for inherited downtimes.
    _compute_inherited_downtime(visitor);

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
  return (_event.get());
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
uint32_t ba::get_id() {
  return (_id);
}

/**
 *  Get the id of the host associated to this ba.
 *
 *  @return  An integer representing the value of this id.
 */
uint32_t ba::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the id of the service associated to this ba.
 *
 *  @return  An integer representing the value of this id.
 */
uint32_t ba::get_service_id() const {
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
  oss << "BA : " << _name
      << " - current_level = " << static_cast<int>(normalize(_level_hard))
      << "%";
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
  if (!_valid)
    state = 3;
  else if (_level_hard <= _level_critical)
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
  if (!_valid)
    state = 3;
  else if (_level_soft <= _level_critical)
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
void ba::remove_impact(std::shared_ptr<kpi> const& impact) {
  std::unordered_map<kpi*, impact_info>::iterator it(
      _impacts.find(impact.get()));
  if (it != _impacts.end()) {
    _unapply_impact(it->second);
    _impacts.erase(it);
  }
  return;
}

/**
 *  Set BA ID.
 *
 *  @param[in] id BA ID.
 */
void ba::set_id(uint32_t id) {
  _id = id;
  return;
}

/**
 *  Set the service id associated to this ba.
 *
 *  @param[in] service_id  Set the service id.
 */
void ba::set_service_id(uint32_t service_id) {
  _service_id = service_id;
}

/**
 *  Set the host id associated to this ba.
 *
 *  @param[in] host_id  Set the service id.
 */
void ba::set_host_id(uint32_t host_id) {
  _host_id = host_id;
}

/**
 *  Set critical level.
 *
 *  @param[in] level  Critical level.
 */
void ba::set_level_critical(double level) {
  _level_critical = level;
  return;
}

/**
 *  Set warning level.
 *
 *  @param[in] level  Warning level.
 */
void ba::set_level_warning(double level) {
  _level_warning = level;
  return;
}

/**
 *  @brief Set the initial, opened event of this ba.
 *
 *  Useful for recovery after cbd stop.
 *
 *  @param[in] event  The event to set.
 */
void ba::set_initial_event(ba_event const& event) {
  if (!_event) {
    _event.reset(new ba_event(event));
    _in_downtime = event.in_downtime;
    _last_kpi_update = _event->start_time;
    _initial_events.push_back(_event);
  }
}

/**
 *  Set the BA name.
 *
 *  @param[in] name  New BA name.
 */
void ba::set_name(std::string const& name) {
  _name = name;
  return;
}

/**
 *  @brief Set whether or not BA is valid.
 *
 *  An invalid BA will return an UNKNOWN state.
 *
 *  @param[in] valid  Whether or not BA is valid.
 */
void ba::set_valid(bool valid) {
  _valid = valid;
  return;
}

/**
 * @brief Set the inherit kpi downtime flag.
 *
 *  @param[in] value  The value to set.
 */
void ba::set_inherit_kpi_downtime(bool value) {
  _inherit_kpi_downtime = value;
}

/**
 *  Visit BA.
 *
 *  @param[out] visitor  Visitor that will receive BA status and events.
 */
void ba::visit(io::stream* visitor) {
  if (visitor) {
    // Commit initial events.
    _commit_initial_events(visitor);

    // If no event was cached, create one if necessary.
    short hard_state(get_state_hard());
    bool state_changed(false);
    if (!_event) {
      if ((_last_kpi_update.get_time_t() == (time_t)-1) ||
          (_last_kpi_update.get_time_t() == (time_t)0))
        _last_kpi_update = time(nullptr);
      _open_new_event(visitor, hard_state);
    }
    // If state changed, close event and open a new one.
    else if ((_in_downtime != _event->in_downtime) ||
             (hard_state != _event->status)) {
      state_changed = true;
      _event->end_time = _last_kpi_update;
      visitor->write(std::static_pointer_cast<io::data>(_event));
      _event.reset();
      _open_new_event(visitor, hard_state);
    }

    // Generate BA status event.
    {
      std::shared_ptr<ba_status> status(new ba_status);
      status->ba_id = _id;
      status->in_downtime = _in_downtime;
      if (_event)
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
      visitor->write(status);
    }

    // Generate virtual service status event.
    if (_generate_virtual_status) {
      std::shared_ptr<neb::service_status> status(new neb::service_status);
      status->active_checks_enabled = false;
      status->check_interval = 0.0;
      status->check_type = 1;  // Passive.
      status->current_check_attempt = 1;
      status->current_state = hard_state;
      status->enabled = true;
      status->event_handler_enabled = false;
      status->execution_time = 0.0;
      status->flap_detection_enabled = false;
      status->has_been_checked = true;
      status->host_id = _host_id;
      // status->host_name = XXX;
      status->is_flapping = false;
      if (_event)
        status->last_check = _event->start_time;
      else
        status->last_check = _last_kpi_update;
      status->last_hard_state = hard_state;
      status->last_hard_state_change = status->last_check;
      status->last_state_change = status->last_check;
      // status->last_time_critical = XXX;
      // status->last_time_unknown = XXX;
      // status->last_time_warning = XXX;
      status->last_update = time(nullptr);
      status->latency = 0.0;
      status->max_check_attempts = 1;
      status->obsess_over = false;
      {
        std::ostringstream oss;
        oss << "BA : Business Activity " << _id
            << " - current_level = " << static_cast<int>(normalize(_level_hard))
            << "%";
        status->output = oss.str();
      }
      // status->percent_state_chagne = XXX;
      {
        std::ostringstream oss;
        oss << "BA_Level=" << static_cast<int>(normalize(_level_hard)) << "%;"
            << static_cast<int>(_level_warning) << ";"
            << static_cast<int>(_level_critical) << ";0;100";
        status->perf_data = oss.str();
      }
      status->retry_interval = 0;
      // status->service_description = XXX;
      status->service_id = _service_id;
      status->should_be_scheduled = false;
      status->state_type = 1;  // Hard.
      visitor->write(status);
    }
  }
  return;
}

/**
 *  @brief Notify BA of a downtime
 *
 *  Used to watch for downtime.
 *
 *  @param dt       Downtime of the service.
 *  @param visitor  Visitor that will receive events.
 */
void ba::service_update(std::shared_ptr<neb::downtime> const& dt,
                        io::stream* visitor) {
  (void)visitor;
  if ((dt->host_id == _host_id) && (dt->service_id == _service_id)) {
    // Log message.
    logging::debug(logging::low)
        << "BAM: BA " << _id
        << " is getting notified of a downtime on its service (" << _host_id
        << ", " << _service_id << ")";

    // Check if there was a change.
    bool in_downtime(dt->was_started &&
                     (dt->actual_end_time == -1 || dt->actual_end_time == 0));
    if (_in_downtime != in_downtime) {
      _in_downtime = in_downtime;

      // Generate status event.
      visit(visitor);

      // Propagate change.
      propagate_update(visitor);
    }
  } else
    logging::error(logging::medium)
        << "BAM: BA " << _id
        << " has got an invalid downtime event."
           " This should never happen. Check your database: got (host "
        << dt->host_id << ", service " << dt->service_id << ") expected ("
        << _host_id << ", " << _service_id << ")";
}

/**
 *  Save the inherited downtime to the cache.
 *
 *  @param[in] cache  The cache.
 */
void ba::save_inherited_downtime(persistent_cache& cache) const {
  if (_inherited_downtime)
    cache.add(std::shared_ptr<inherited_downtime>(
        new inherited_downtime(*_inherited_downtime)));
}

/**
 *  Set the inherited downtime of this ba.
 *
 *  @param[in] dwn  The inherited downtime.
 */
void ba::set_inherited_downtime(inherited_downtime const& dwn) {
  _inherited_downtime.reset(new inherited_downtime(dwn));
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

  return;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void ba::_internal_copy(ba const& other) {
  _acknowledgement_hard = other._acknowledgement_hard;
  _acknowledgement_soft = other._acknowledgement_soft;
  _downtime_hard = other._downtime_hard;
  _downtime_soft = other._downtime_soft;
  _event = other._event;
  _generate_virtual_status = other._generate_virtual_status;
  _id = other._id;
  _service_id = other._service_id;
  _host_id = other._host_id;
  _impacts = other._impacts;
  _in_downtime = other._in_downtime;
  _last_kpi_update = other._last_kpi_update;
  _level_critical = other._level_critical;
  _level_hard = other._level_hard;
  _level_soft = other._level_soft;
  _level_warning = other._level_warning;
  _name = other._name;
  _valid = other._valid;
  return;
}

/**
 *  Open a new event for this BA.
 *
 *  @param[out] visitor             Visitor that will receive events.
 *  @param[in]  service_hard_state  Hard state of virtual BA service.
 */
void ba::_open_new_event(io::stream* visitor, short service_hard_state) {
  _event.reset(new ba_event);
  _event->ba_id = _id;
  _event->first_level = _level_hard < 0 ? 0 : _level_hard;
  _event->in_downtime = _in_downtime;
  _event->status = service_hard_state;
  _event->start_time = _last_kpi_update;
  if (visitor) {
    std::shared_ptr<io::data> be(new ba_event(*_event));
    visitor->write(be);
  }
  return;
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
  for (std::unordered_map<kpi*, impact_info>::iterator it(_impacts.begin()),
       end(_impacts.end());
       it != end; ++it)
    _apply_impact(it->second);
  _recompute_count = 0;
  return;
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

  return;
}

/**
 * Commit the initial events of this ba.
 *
 *  @param[in] visitor  The visitor.
 */
void ba::_commit_initial_events(io::stream* visitor) {
  if (_initial_events.empty())
    return;

  if (visitor) {
    for (std::vector<std::shared_ptr<ba_event> >::const_iterator
             it(_initial_events.begin()),
         end(_initial_events.end());
         it != end; ++it)
      visitor->write(std::shared_ptr<io::data>(new ba_event(**it)));
  }
  _initial_events.clear();
}

/**
 *  Compute the inherited downtime.
 */
void ba::_compute_inherited_downtime(io::stream* visitor) {
  // kpi downtime heritance deactived. Do nothing.
  if (!_inherit_kpi_downtime)
    return;

  // Check if every impacting child KPIs are in downtime.
  bool every_kpi_in_downtime(!_impacts.empty());
  for (std::unordered_map<kpi*, impact_info>::const_iterator
           it = _impacts.begin(),
           end = _impacts.end();
       it != end; ++it) {
    if (!it->first->ok_state() && !it->first->in_downtime()) {
      every_kpi_in_downtime = false;
      break;
    }
  }

  // Case 1: state not ok, every child in downtime, no actual downtime.
  //         Put the BA in downtime.
  bool state_ok(!get_state_hard());
  if (!state_ok && every_kpi_in_downtime && !_inherited_downtime) {
    _inherited_downtime.reset(new inherited_downtime);
    _inherited_downtime->ba_id = _id;
    _inherited_downtime->in_downtime = true;
    _in_downtime = true;

    if (visitor)
      visitor->write(std::shared_ptr<inherited_downtime>(
          new inherited_downtime(*_inherited_downtime)));
  }
  // Case 2: state ok or not every kpi in downtime, actual downtime.
  //         Remove the downtime.
  else if ((state_ok || !every_kpi_in_downtime) && _inherited_downtime.get()) {
    _inherited_downtime.reset();
    if (visitor) {
      std::shared_ptr<inherited_downtime> dwn(new inherited_downtime);
      dwn->ba_id = _id;
      dwn->in_downtime = false;
      visitor->write(dwn);
    }
  }
}
