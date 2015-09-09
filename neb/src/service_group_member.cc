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
#include "com/centreon/broker/neb/service_group_member.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
service_group_member::service_group_member() : service_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] sgm Object to copy.
 */
service_group_member::service_group_member(service_group_member const& sgm)
  : group_member(sgm), service_id(sgm.service_id) {}

/**
 *  Destructor.
 */
service_group_member::~service_group_member() {}

/**
 *  Assignement operator.
 *
 *  @param[in] sgm Object to copy.
 *
 *  @return This object.
 */
service_group_member& service_group_member::operator=(service_group_member const& sgm) {
  group_member::operator=(sgm);
  service_id = sgm.service_id;
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int service_group_member::type() const {
  return (io::events::data_type<io::events::neb, neb::de_service_group_member>::value);
}
