/*
** Copyright 2018 Centreon
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

#include "com/centreon/broker/neb/responsive_instance.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
responsive_instance::responsive_instance() : poller_id(0), responsive(true) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
responsive_instance::responsive_instance(responsive_instance const& i)
    : io::data(i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
responsive_instance::~responsive_instance() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
responsive_instance& responsive_instance::operator=(
    responsive_instance const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
uint32_t responsive_instance::type() const {
  return (io::events::data_type<io::events::neb,
                                neb::de_responsive_instance>::value);
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const responsive_instance::entries[] = {
    mapping::entry(&responsive_instance::poller_id, "poller_id"),
    mapping::entry(&responsive_instance::responsive, "responsive"),
    mapping::entry()};

// Operations.
static io::data* new_im() {
  return (new responsive_instance);
}
io::event_info::event_operations const responsive_instance::operations = {
    &new_im};

/**************************************
 *                                     *
 *          Private Methods            *
 *                                     *
 **************************************/

/**
 *  @brief Copy internal data of the object to the current one.
 *
 *  Copy data defined within the responsive_instance class. This method is used
 * by the copy constructor and the assignment operator.
 *
 *  @param[in] other Object to copy.
 */
void responsive_instance::_internal_copy(responsive_instance const& other) {
  poller_id = other.poller_id;
  responsive = other.responsive;
}
