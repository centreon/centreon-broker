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
#include "com/centreon/broker/neb/host_dependency.hh"
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
host_dependency::host_dependency() {}

/**
 *  Copy constructor.
 *
 *  @param[in] hd Object to copy.
 */
host_dependency::host_dependency(host_dependency const& hd)
  : dependency(hd) {}

/**
 *  Destructor.
 */
host_dependency::~host_dependency() {}

/**
 *  Assignment operator.
 *
 *  @param[in] hd Object to copy.
 *
 *  @return This object.
 */
host_dependency& host_dependency::operator=(host_dependency const& hd) {
  dependency::operator=(hd);
  return (*this);
}

/**
 *  Get the type of this object.
 *
 *  @return The event type.
 */
unsigned int host_dependency::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_dependency>::value);
}
