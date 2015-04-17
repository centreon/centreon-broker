/*
** Copyright 2011-2013,2015 Merethis
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

#include "com/centreon/broker/correlation/engine_state.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
engine_state::engine_state() : instance_id(0), started(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] es Object to copy.
 */
engine_state::engine_state(engine_state const& es)
  : io::data(es), instance_id(es.instance_id), started(es.started) {}

/**
 *  Destructor.
 */
engine_state::~engine_state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] es Object to copy.
 *
 *  @return This object.
 */
engine_state& engine_state::operator=(engine_state const& es) {
  if (this != &es) {
    io::data::operator=(es);
    instance_id = es.instance_id;
    started = es.started;
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int engine_state::type() const {
  return (engine_state::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int engine_state::static_type() {
  return (io::events::data_type<io::events::correlation, correlation::de_engine_state>::value);
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const engine_state::entries[] = {
  mapping::entry(
    &engine_state::started,
    "started"),
  mapping::entry()
};

// Operations.
static io::data* new_engine_state() {
  return (new engine_state);
}
io::event_info::event_operations const engine_state::operations = {
  &new_engine_state
};
