/*
** Copyright 2017 Centreon
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

#include "com/centreon/broker/generator/dummy.hh"
#include "com/centreon/broker/generator/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::generator;

/**
 *  Constructor.
 *
 *  @param[in] n  Number value.
 */
dummy::dummy(uint32_t n) : number(n) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dummy::dummy(dummy const& other) : io::data(other), number(other.number) {}

/**
 *  Destructor.
 */
dummy::~dummy() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dummy& dummy::operator=(dummy const& other) {
  if (this != &other) {
    io::data::operator=(other);
    number = other.number;
  }
  return (*this);
}

/**
 *  @brief Get the type of the event.
 *
 *  Return the type of this event. This can be useful for runtime data
 *  type determination.
 *
 *  @return The event type.
 */
uint32_t dummy::type() const {
  return (dummy::static_type());
}

// Mapping.
mapping::entry const dummy::entries[] = {
    mapping::entry(&dummy::number, "number"), mapping::entry()};

// Operations.
static io::data* new_dummy() {
  return (new dummy());
}
io::event_info::event_operations const dummy::operations = {&new_dummy};
