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
#include "com/centreon/broker/neb/custom_variable_status.hh"
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
custom_variable_status::custom_variable_status()
  : host_id(0),
        modified(true),
    service_id(0),
    update_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] cvs Object to copy.
 */
custom_variable_status::custom_variable_status(custom_variable_status const& cvs)
  : io::data(cvs) {
  _internal_copy(cvs);
}

/**
 *  Destructor.
 */
custom_variable_status::~custom_variable_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] cvs Object to copy.
 *
 *  @return This object.
 */
custom_variable_status& custom_variable_status::operator=(custom_variable_status const& cvs) {
  io::data::operator=(cvs);
  _internal_copy(cvs);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event type.
 */
unsigned int custom_variable_status::type() const {
  return (io::events::data_type<io::events::neb, neb::de_custom_variable_status>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] cvs Object to copy.
 */
void custom_variable_status::_internal_copy(custom_variable_status const& cvs) {
  host_id = cvs.host_id;
    modified = cvs.modified;
  name = cvs.name;
  service_id = cvs.service_id;
  update_time = cvs.update_time;
  value = cvs.value;
  return ;
}
