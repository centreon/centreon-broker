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
#include "com/centreon/broker/neb/host_parent.hh"
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
host_parent::host_parent()
  : enabled(true),
    host_id(0),
    parent_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] hp Object to copy.
 */
host_parent::host_parent(host_parent const& hp)
  : io::data(hp),
    enabled(hp.enabled),
    host_id(hp.host_id),
    parent_id(hp.parent_id) {}

/**
 *  Destructor.
 */
host_parent::~host_parent() {}

/**
 *  Assignment operator.
 *
 *  @param[in] hp Object to copy.
 *
 *  @return This object.
 */
host_parent& host_parent::operator=(host_parent const& hp) {
  io::data::operator=(hp);
  enabled = hp.enabled;
  host_id = hp.host_id;
    parent_id = hp.parent_id;
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int host_parent::type() const {
  return (io::events::data_type<io::events::neb, neb::de_host_parent>::value);
}
