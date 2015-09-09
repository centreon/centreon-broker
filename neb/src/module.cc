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
#include "com/centreon/broker/neb/module.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
module::module()
  : enabled(true),
        loaded(false),
    should_be_loaded(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] m Object to copy.
 */
module::module(module const& m) : io::data(m) {
  _internal_copy(m);
}

/**
 *  Destructor.
 */
module::~module() {}

/**
 *  Assignment operator.
 *
 *  @param[in] m Object to copy.
 *
 *  @return This object.
 */
module& module::operator=(module const& m) {
  io::data::operator=(m);
  _internal_copy(m);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The event_type.
 */
unsigned int module::type() const {
  return (io::events::data_type<io::events::neb, neb::de_module>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy data members.
 *
 *  @param[in] m Object to copy.
 */
void module::_internal_copy(module const& m) {
  args = m.args;
  enabled = m.enabled;
  filename = m.filename;
    loaded = m.loaded;
  should_be_loaded = m.should_be_loaded;
  return ;
}
