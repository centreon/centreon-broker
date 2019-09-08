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

#include "com/centreon/broker/bam/kpi_boolexp.hh"
#include "com/centreon/broker/bam/bool_expression.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/kpi_status.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_boolexp::kpi_boolexp() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
kpi_boolexp::kpi_boolexp(kpi_boolexp const& other) : kpi(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
kpi_boolexp::~kpi_boolexp() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
kpi_boolexp& kpi_boolexp::operator=(kpi_boolexp const& other) {
  if (this != &other) {
    kpi::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Base boolean expression got updated.
 *
 *  @param[in]  child    Boolean expression that got updated.
 *  @param[out] visitor  Receive events generated by this object.
 *
 *  @return True if the values of this object were modified.
 */
bool kpi_boolexp::child_has_update(computable* child, io::stream* visitor) {
  // It is useless to maintain a cache of boolean expression values in
  // this class, as the bool_expression class already cache most of them.
  if (child == _boolexp.get()) {
    // Logging.
    logging::debug(logging::low) << "BAM: boolean expression KPI " << _id
                                 << " is getting notified of child update";

    // Generate status event.
    visit(visitor);
  }
  return (true);
}

/**
 *  Return true if in downtime.
 *
 *  @return  True or false.
 */
bool kpi_boolexp::in_downtime() const {
  if (_boolexp)
    return (_boolexp->in_downtime());
  return (false);
}

/**
 *  Get the impact introduced when the boolean expression is triggered.
 *
 *  @return Impact if the boolean expression is triggered.
 */
double kpi_boolexp::get_impact() const {
  return (_impact);
}

/**
 *  Get the hard impact introduced by the boolean expression.
 *
 *  @param[out] hard_impact  Hard impacts.
 */
void kpi_boolexp::impact_hard(impact_values& hard_impact) {
  _fill_impact(hard_impact);
  return;
}

/**
 *  Get the soft impact introduced by the boolean expression.
 *
 *  @param[out] soft_impact  Soft impacts.
 */
void kpi_boolexp::impact_soft(impact_values& soft_impact) {
  _fill_impact(soft_impact);
  return;
}

/**
 *  Link the kpi_boolexp with a specific boolean expression (class
 *  bool_expression).
 *
 *  @param[in] my_boolexp  Linked boolean expression.
 */
void kpi_boolexp::link_boolexp(std::shared_ptr<bool_expression>& my_boolexp) {
  _boolexp = my_boolexp;
  return;
}

/**
 *  Set impact if the boolean expression is triggered.
 *
 *  @param[in] impact  Impact if the boolean expression is triggered.
 */
void kpi_boolexp::set_impact(double impact) {
  _impact = impact;
  return;
}

/**
 *  Unlink from boolean expression.
 */
void kpi_boolexp::unlink_boolexp() {
  _boolexp.reset();
}

/**
 *  Visit boolean expression KPI.
 *
 *  @param[out] visitor  Object that will receive status and events.
 */
void kpi_boolexp::visit(io::stream* visitor) {
  if (visitor) {
    // Commit the initial events saved in the cache.
    commit_initial_events(visitor);

    // Get information (HARD and SOFT values are the same).
    impact_values values;
    impact_hard(values);
    short state(_get_state());

    // Generate BI events.
    {
      // If no event was cached, create one.
      if (!_event)
        _open_new_event(visitor, values.get_nominal(), state);
      // If state changed, close event and open a new one.
      else if (state != _event->status) {
        _event->end_time = ::time(nullptr);
        visitor->write(std::static_pointer_cast<io::data>(_event));
        _event.reset();
        _open_new_event(visitor, values.get_nominal(), state);
      }
    }

    // Generate status event.
    {
      std::shared_ptr<kpi_status> status(new kpi_status);
      status->kpi_id = _id;
      status->in_downtime = in_downtime();
      status->level_acknowledgement_hard = values.get_acknowledgement();
      status->level_acknowledgement_soft = values.get_acknowledgement();
      status->level_downtime_hard = values.get_downtime();
      status->level_downtime_soft = values.get_downtime();
      status->level_nominal_hard = values.get_nominal();
      status->level_nominal_soft = values.get_nominal();
      status->state_hard = state;
      status->state_soft = state;
      status->last_state_change = get_last_state_change();
      status->last_impact = values.get_nominal();
      visitor->write(std::static_pointer_cast<io::data>(status));
    }
  }
}

/**
 *  Fill impact_values from base values.
 *
 *  @param[out] impact  Impact values.
 */
void kpi_boolexp::_fill_impact(impact_values& impact) {
  // Get nominal impact from state.
  short state(_get_state());
  double nominal;
  if (0 == state)
    nominal = 0.0;
  else
    nominal = _impact;
  impact.set_nominal(nominal);
  impact.set_acknowledgement(0.0);
  impact.set_downtime(0.0);
  return;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void kpi_boolexp::_internal_copy(kpi_boolexp const& other) {
  _boolexp = other._boolexp;
  _event = other._event;
  _impact = other._impact;
  return;
}

/**
 *  Open a new event for this KPI.
 *
 *  @param[out] visitor  Visitor that will receive events.
 *  @param[in]  impact   Current impact of this KPI.
 *  @param[in]  state    Boolean expression state.
 */
void kpi_boolexp::_open_new_event(io::stream* visitor,
                                  int impact,
                                  short state) {
  _event.reset(new kpi_event);
  _event->kpi_id = _id;
  _event->impact_level = impact;
  _event->in_downtime = false;
  _event->output = "BAM boolean expression computed by Centreon Broker";
  _event->perfdata = "";
  _event->start_time = time(nullptr);
  _event->status = state;
  if (visitor) {
    std::shared_ptr<io::data> ke(new kpi_event(*_event));
    visitor->write(ke);
  }
  return;
}

/**
 *  @brief Get the current state of the boolexp.
 *
 *  A boolean expression can be uninitialized yet, if a service status
 *  has yet to come. If this is the case, the status is the one of the
 *  opened event.
 *
 *  @return  The current state of the boolexp.
 */
short kpi_boolexp::_get_state() const {
  if (_boolexp->state_known())
    return (_boolexp->get_state());
  else {
    if (_event)
      return (_event->status);
    else
      return (_boolexp->get_state());
  }
}

/**
 *  Is this KPI in an ok state?
 *
 *  @return  True if this KPI is in an ok state.
 */
bool kpi_boolexp::ok_state() const {
  return (_get_state() == 0);
}
