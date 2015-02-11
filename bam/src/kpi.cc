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

#include <ctime>
#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/kpi.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi::kpi() : _id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
kpi::kpi(kpi const& right) :
  computable(right),
  _id(right._id),
  _event(right._event) {}

/**
 *  Destructor.
 */
kpi::~kpi() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
kpi& kpi::operator=(kpi const& right) {
  if (this != &right) {
    computable::operator=(right);
    _id = right._id;
    _event = right._event;
  }
  return (*this);
}

/**
 *  Get KPI ID.
 *
 *  @return KPI ID.
 */
unsigned int kpi::get_id() const {
  return (_id);
}

/**
 *  Get the last state change.
 *
 *  @return Last state change.
 */
timestamp kpi::get_last_state_change() const {
  return (!_event.isNull() ? _event->start_time : timestamp(time(NULL)));
}

/**
 *  Set KPI ID.
 *
 *  @param[in] id KPI ID.
 */
void kpi::set_id(unsigned int id) {
  _id = id;
  return ;
}

/**
 *  Set the initial event of the kpi.
 *
 *  @param[in] e  The kpi event.
 */
void kpi::set_initial_event(kpi_event const& e) {
  if (_event.isNull()) {
    _event = misc::shared_ptr<kpi_event>(new kpi_event(e));
    impact_values impacts;
    impact_hard(impacts);
    double new_impact_level =
             _event->in_downtime
               ? impacts.get_downtime()
               : impacts.get_nominal();
    // If the new impact is not equal to the impact saved in the initial event,
    // then close the initial event and open a new event.
    if (new_impact_level != _event->impact_level
          && _event->impact_level != -1) {
      time_t now = ::time(NULL);
      misc::shared_ptr<kpi_event> new_event(new kpi_event(e));
      new_event->end_time = now;
      _initial_events.push_back(new_event);
      new_event = misc::shared_ptr<kpi_event> (new kpi_event(e));
      new_event->start_time = now;
      _initial_events.push_back(new_event);
      _event = new_event;
    }
    else
      _initial_events.push_back(_event);;
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
    return ;

  if (visitor) {
    for (std::vector<misc::shared_ptr<kpi_event> >::const_iterator
           it(_initial_events.begin()),
           end(_initial_events.end());
         it != end;
         ++it)
      visitor->write(misc::shared_ptr<io::data>(new kpi_event(**it)));
  }
  _initial_events.clear();
}
