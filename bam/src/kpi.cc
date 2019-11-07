/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/kpi.hh"
#include <ctime>
#include "com/centreon/broker/bam/ba.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi::kpi() : _id(0) {}

/**
 *  Destructor.
 */
kpi::~kpi() {}

/**
 *  Get KPI ID.
 *
 *  @return KPI ID.
 */
uint32_t kpi::get_id() const {
  return (_id);
}

/**
 *  Get the last state change.
 *
 *  @return Last state change.
 */
timestamp kpi::get_last_state_change() const {
  return (_event ? _event->start_time : timestamp(time(nullptr)));
}

/**
 *  Set KPI ID.
 *
 *  @param[in] id KPI ID.
 */
void kpi::set_id(uint32_t id) {
  _id = id;
  return;
}

/**
 *  Set the initial event of the kpi.
 *
 *  @param[in] e  The kpi event.
 */
void kpi::set_initial_event(kpi_event const& e) {
  if (!_event) {
    _event.reset(new kpi_event(e));
    impact_values impacts;
    impact_hard(impacts);
    double new_impact_level =
        _event->in_downtime ? impacts.get_downtime() : impacts.get_nominal();
    // If the new impact is not equal to the impact saved in the initial event,
    // then close the initial event and open a new event.
    if (new_impact_level != _event->impact_level &&
        _event->impact_level != -1) {
      time_t now = ::time(nullptr);
      std::shared_ptr<kpi_event> new_event(new kpi_event(e));
      new_event->end_time = now;
      _initial_events.push_back(new_event);
      new_event = std::shared_ptr<kpi_event>(new kpi_event(e));
      new_event->start_time = now;
      _initial_events.push_back(new_event);
      _event = new_event;
    } else
      _initial_events.push_back(_event);
    ;
    _event->impact_level = new_impact_level;
  }
}

/**
 * Commit the initial events of this kpi.
 *
 *  @param[in] visitor  The visitor.
 */
void kpi::commit_initial_events(io::stream* visitor) {
  if (_initial_events.empty())
    return;

  if (visitor) {
    for (std::vector<std::shared_ptr<kpi_event> >::const_iterator
             it(_initial_events.begin()),
         end(_initial_events.end());
         it != end; ++it)
      visitor->write(std::shared_ptr<io::data>(new kpi_event(**it)));
  }
  _initial_events.clear();
}

/**
 *  Is this kpi in downtime?
 *
 *  @return  Default value: false.
 */
bool kpi::in_downtime() const {
  return (false);
}
