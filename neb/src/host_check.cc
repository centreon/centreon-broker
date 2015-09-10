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
#include "com/centreon/broker/neb/host_check.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
host_check::host_check() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hc Object to copy.
 */
host_check::host_check(host_check const& hc) : check(hc) {}

/**
 *  Destructor.
 */
host_check::~host_check() {}

/**
 *  Assignment operator.
 *
 *  @param[in] hc Object to copy.
 *
 *  @return This object.
 */
host_check& host_check::operator=(host_check const& hc) {
  check::operator=(hc);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int host_check::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_check>::value);
}
