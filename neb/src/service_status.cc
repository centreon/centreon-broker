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
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
service_status::service_status()
  : last_time_critical(0),
    last_time_ok(0),
    last_time_unknown(0),
    last_time_warning(0),
    service_id(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given service_status to the current
 *  instance.
 *
 *  @param[in] ss Object to copy.
 */
service_status::service_status(service_status const& ss)
  : host_service_status(ss) {
  _internal_copy(ss);
}

/**
 *  Destructor.
 */
service_status::~service_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given service_status to the current
 *  instance.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
service_status& service_status::operator=(service_status const& ss) {
  host_service_status::operator=(ss);
  _internal_copy(ss);
  return (*this);
}

/**
 *  Returns the type of the event.
 *
 *  @return The event_type.
 */
unsigned int service_status::type() const {
  return (io::events::data_type<io::events::neb, neb::de_service_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal members of the given object to the current
 *         instance.
 *
 *  Make a copy of all internal members defined within service_status to
 *  the current instance. This method is used by the copy constructor
 *  and the assignment operator.
 *
 *  @param[in] ss Object to copy.
 */
void service_status::_internal_copy(service_status const& ss) {
  host_name = ss.host_name;
  last_time_critical = ss.last_time_critical;
  last_time_ok = ss.last_time_ok;
  last_time_unknown = ss.last_time_unknown;
  last_time_warning = ss.last_time_warning;
  service_description = ss.service_description;
  service_id = ss.service_id;
  return ;
}
