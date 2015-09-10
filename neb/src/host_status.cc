/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  Initialize all members to 0, NULL or equivalent.
 */
host_status::host_status()
  : last_time_down(0),
    last_time_unreachable(0),
    last_time_up(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] hs Object to copy.
 */
host_status::host_status(host_status const& hs)
  : host_service_status(hs) {
  _internal_copy(hs);
}

/**
 *  Destructor.
 */
host_status::~host_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all internal data of the given object to the current instance.
 *
 *  @param[in] hs Object to copy.
 *
 *  @return This object.
 */
host_status& host_status::operator=(host_status const& hs) {
  host_service_status::operator=(hs);
  _internal_copy(hs);
  return (*this);
}

/**
 *  Returns the type of the event.
 *
 *  @return The event_type.
 */
unsigned int host_status::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy all internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members of HostStatus to the current
 *  instance. This method is use by the copy constructor and the
 *  assignment operator.
 *
 *  @param[in] hs Object to copy.
 */
void host_status::_internal_copy(host_status const& hs) {
  last_time_down = hs.last_time_down;
  last_time_unreachable = hs.last_time_unreachable;
  last_time_up = hs.last_time_up;
  return ;
}
