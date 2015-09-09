/*
** Copyright 2011-2013 Centreon
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
  return (io::events::data_type<io::events::correlation, correlation::de_engine_state>::value);
}
