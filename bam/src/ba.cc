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
#include "com/centreon/broker/bam/event_parent.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi.hh"
#include "com/centreon/broker/bam/stream.hh"
#include "com/centreon/broker/logging/logging.hh"

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
    _id(0),
    _service_id(0),
    _level_critical(0.0),
    _level_hard(100.0),
    _level_soft(100.0),
    _level_warning(0.0),
    _recompute_count(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
ba::ba(ba const& right) : computable(right) {
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
bool ba::child_has_update(computable* child, stream* visitor) {
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
    _generate_events(*static_cast<kpi*>(child), visitor);
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
 *  Get the id of the service associated to this ba.
 *
 *  @return  An integer representing the value of this id.
 */
unsigned int ba::get_service_id() const {
  return (_service_id);
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
 *  Visit BA.
 *
 *  @param[in]  kpi_obj  Kpi that was updated.
 *  @param[out] visitor  Visitor that will receive BA status and events.
 */
void ba::visit(stream* visitor) {
  if (visitor) {
    misc::shared_ptr<ba_status> status(new ba_status);
    status->ba_id = _id;
    status->level_acknowledgement = normalize(_acknowledgement_hard);
    status->level_downtime = normalize(_downtime_hard);
    status->level_nominal = normalize(_level_hard);
    visitor->write(status.staticCast<io::data>());
  }
  return ;
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
  _id = right._id;
  _service_id = right._service_id;
  _impacts = right._impacts;
  _level_critical = right._level_critical;
  _level_hard = right._level_hard;
  _level_soft = right._level_soft;
  _level_warning = right._level_warning;
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
 *  Open a new event for this ba.
 */
void ba::_open_new_event() {
  _event = new(ba_event);

  _event->ba_id = _id;
  _event->start_time = time(NULL);
  _event->status = get_state_hard();
}

/**
 *  Generate a ba event and its parenting with a kpi event.
 *
 *  @param[in] kpi_obj    The kpi that has been updated.
 *  @param[out] visitor   The stream to write the event to.
 */
void ba::_generate_events(kpi const& kpi_obj,
                          stream* visitor) {
  if (!visitor)
    return;

  // If no event was cached, create one.
  if (_event.isNull()) {
    _open_new_event();
    return;
  }

  // If the status was changed, close the current event, write it
  // and create a new one
  short actual_status = get_state_hard();
  if (actual_status != _event->status) {
    _event->duration = std::difftime(time(NULL), _event->start_time);
    visitor->write(_event.staticCast<io::data>());

    // Generate the event parenting with the kpi.
    misc::shared_ptr<event_parent> parent_event(new event_parent);
    parent_event->kpi_id = kpi_obj.get_id();
    parent_event->kpi_start_time = _event->start_time;
    parent_event->ba_id = _id;
    parent_event->ba_start_time = _event->start_time;
    visitor->write(parent_event.staticCast<io::data>());

    // Open a new event.
    _open_new_event();
  }
}
