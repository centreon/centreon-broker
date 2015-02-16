/*
** Copyright 2009-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/module.hh"

using namespace com::centreon::broker;
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

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const module::entries[] = {
  mapping::entry(
    &module::args,
    "args",
    1),
  mapping::entry(
    &module::enabled,
    "",
    6),
  mapping::entry(
    &module::filename,
    "filename",
    2),
  mapping::entry(
    &module::instance_id,
    "instance_id",
    3,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &module::loaded,
    "loaded",
    4),
  mapping::entry(
    &module::should_be_loaded,
    "should_be_loaded",
    5),
  mapping::entry()
};

// Operations.
static io::data* new_module() {
  return (new module);
}
io::event_info::event_operations const module::operations = {
  &new_module
};

