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

#include "com/centreon/broker/bam/kpi_ba.hh"
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_ba::kpi_ba() {}

/**
 *  Destructor.
 */
kpi_ba::~kpi_ba() {}

/**
 *  Base BA got updated.
 *
 *  @param[in]  child    BA that got updated.
 *  @param[out] visitor  Receive events generated by this object.
 *
 *  @return True if the values of this object were modified.
 */
bool kpi_ba::child_has_update(computable* child, io::stream* visitor) {
  // It is useless to maintain a cache of BA values in this class, as
  // the ba class already cache most of them.
  if (child == _ba.get()) {
    // Logging.
    log_v2::bam()->debug("BAM: BA KPI {} is getting notified of child update",
                         _id);
    logging::debug(logging::low)
        << "BAM: BA KPI " << _id << " is getting notified of child update";

    // Generate status event.
    visit(visitor);
  }
  return true;
}

/**
 *  Get the impact introduced by a CRITICAL state of the BA.
 *
 *  @return Impact if BA is CRITICAL.
 */
double kpi_ba::get_impact_critical() const {
  return (_impact_critical);
}

/**
 *  Get the impact introduced by a WARNING state of the BA.
 *
 *  @return Impact if BA is WARNING.
 */
double kpi_ba::get_impact_warning() const {
  return (_impact_warning);
}

/**
 *  Get the hard impact introduced by the BA.
 *
 *  @param[out] hard_impact Hard impacts.
 */
void kpi_ba::impact_hard(impact_values& hard_impact) {
  _fill_impact(hard_impact, _ba->get_state_hard(), _ba->get_ack_impact_hard(),
               _ba->get_downtime_impact_hard());
  return;
}

/**
 *  Get the soft impact introduced by the BA.
 *
 *  @param[out] soft_impact Soft impacts.
 */
void kpi_ba::impact_soft(impact_values& soft_impact) {
  _fill_impact(soft_impact, _ba->get_state_soft(), _ba->get_ack_impact_soft(),
               _ba->get_downtime_impact_soft());
  return;
}

/**
 *  Link the kpi_ba with a specific BA (class ba).
 *
 *  @param[in] my_ba Linked BA.
 */
void kpi_ba::link_ba(std::shared_ptr<ba>& my_ba) {
  _ba = my_ba;
  return;
}

/**
 *  Set impact if BA is CRITICAL.
 *
 *  @param[in] impact Impact if BA is CRITICAL.
 */
void kpi_ba::set_impact_critical(double impact) {
  _impact_critical = impact;
  return;
}

/**
 *  Set impact if BA is WARNING.
 *
 *  @param[in] impact Impact if BA is WARNING.
 */
void kpi_ba::set_impact_warning(double impact) {
  _impact_warning = impact;
  return;
}

/**
 *  Set impact if BA is UNKNOWN.
 *
 *  @param[in] impact Impact if BA is UNKNOWN.
 */
void kpi_ba::set_impact_unknown(double impact) {
  _impact_unknown = impact;
}

/**
 *  Unlink from BA.
 */
void kpi_ba::unlink_ba() {
  _ba.reset();
}

/**
 *  Visit BA KPI.
 *
 *  @param[out] visitor  Object that will receive status and events.
 */
void kpi_ba::visit(io::stream* visitor) {
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
      // BA event state.
      ba_event* bae(_ba->get_ba_event());
      kpi_ba::state ba_state(bae ? static_cast<kpi_ba::state>(bae->status)
                                 : kpi_ba::state::state_ok);
      timestamp last_ba_update(bae ? bae->start_time
                                   : timestamp(time(nullptr)));

      // If no event was cached, create one.
      if (!_event) {
        if ((last_ba_update.get_time_t() != (time_t)-1) &&
            (last_ba_update.get_time_t() != (time_t)0))
          _open_new_event(visitor, hard_values.get_nominal(), ba_state,
                          last_ba_update);
      }
      // If state changed, close event and open a new one.
      else if ((_ba->get_in_downtime() != _event->in_downtime) ||
               (ba_state != _event->status)) {
        _event->end_time = last_ba_update;
        visitor->write(std::static_pointer_cast<io::data>(_event));
        _event.reset();
        _open_new_event(visitor, hard_values.get_nominal(), ba_state,
                        last_ba_update);
      }
    }

    // Generate status event.
    {
      std::shared_ptr<kpi_status> status{std::make_shared<kpi_status>(_id)};
      status->level_acknowledgement_hard = hard_values.get_acknowledgement();
      status->level_acknowledgement_soft = soft_values.get_acknowledgement();
      status->level_downtime_hard = hard_values.get_downtime();
      status->level_downtime_soft = soft_values.get_downtime();
      status->level_nominal_hard = hard_values.get_nominal();
      status->level_nominal_soft = soft_values.get_nominal();
      status->state_hard = _ba->get_state_hard();
      status->state_soft = _ba->get_state_soft();
      status->last_state_change = get_last_state_change();
      status->last_impact = hard_values.get_nominal();
      visitor->write(std::static_pointer_cast<io::data>(status));
    }
  }
}

/**
 *  Fill impact_values from base values.
 *
 *  @param[out] impact          Impact values.
 *  @param[in]  state           BA state.
 *  @param[in]  acknowledgement Acknowledgement impact of the BA.
 *  @param[in]  downtime        Downtime impact of the BA.
 */
void kpi_ba::_fill_impact(impact_values& impact,
                          kpi_ba::state state,
                          double acknowledgement,
                          double downtime) {
  // Get nominal impact from state.
  double nominal;
  switch (state) {
    case 0:
      nominal = 0.0;
      break;
    case 1:
      nominal = _impact_warning;
      break;
    case 2:
      nominal = _impact_critical;
      break;
    default:
      nominal = _impact_unknown;
      break;
  }
  impact.set_nominal(nominal);

  // Compute acknowledged and downtimed impacts. Acknowledgement and
  // downtime impacts provided as arguments are from the BA. Therefore
  // are used to proportionnaly compute the acknowledged and downtimed
  // impacts, relative to the nominal impact.
  if (acknowledgement < 0.0)
    acknowledgement = 0.0;
  else if (acknowledgement > 100.0)
    acknowledgement = 100.0;
  impact.set_acknowledgement(acknowledgement * nominal / 100.0);
  if (downtime < 0.0)
    downtime = 0.0;
  else if (downtime > 100.0)
    downtime = 100.0;
  impact.set_downtime(downtime * nominal / 100.0);
  impact.set_state(state);
}

/**
 *  Open a new event for this KPI.
 *
 *  @param[out] visitor           Visitor that will receive events.
 *  @param[in]  impact            Current impact of this KPI.
 *  @param[in]  ba_state          BA state.
 *  @param[in]  event_start_time  Event start time.
 */
void kpi_ba::_open_new_event(io::stream* visitor,
                             int impact,
                             kpi_ba::state ba_state,
                             timestamp event_start_time) {
  _event.reset(new kpi_event);
  _event->kpi_id = _id;
  _event->ba_id = _ba_id;
  _event->impact_level = impact;
  _event->in_downtime = _ba->get_in_downtime();
  _event->output = _ba->get_output();
  _event->perfdata = _ba->get_perfdata();
  _event->start_time = event_start_time;
  _event->status = ba_state;
  if (visitor) {
    std::shared_ptr<io::data> ke(new kpi_event(*_event));
    visitor->write(ke);
  }
  return;
}

/**
 *  Is this KPI in an ok state?
 *
 *  @return  True if this KPI is in an ok state.
 */
bool kpi_ba::ok_state() const {
  return (_ba->get_state_hard() == 0);
}

/**
 *  Is this KPI in downtime?
 *
 *  @return  True if this KPI is in downtime.
 */
bool kpi_ba::in_downtime() const {
  return (_ba->get_in_downtime());
}
