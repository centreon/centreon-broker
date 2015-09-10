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
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/internal.hh"

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
instance::instance()
  : id(0),
    is_running(true),
    pid(0),
    program_end(0),
    program_start(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
instance::instance(instance const& i) : io::data(i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
instance::~instance() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
instance& instance::operator=(instance const& i) {
  io::data::operator=(i);
  _internal_copy(i);
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
unsigned int instance::type() const {
  return (io::events::data_type<io::events::neb, neb::de_instance>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the instance object to the current
 *         instance.
 *
 *  Copy data defined within the instance class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  @param[in] i Object to copy.
 */
void instance::_internal_copy(instance const& i) {
  engine = i.engine;
  id = i.id;
  is_running = i.is_running;
  name = i.name;
  pid = i.pid;
  program_end = i.program_end;
  program_start = i.program_start;
  version = i.version;
  return ;
}
