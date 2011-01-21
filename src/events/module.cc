/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "events/module.hh"

using namespace events;

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
  filename = m.filename;
  instance_id = m.instance_id;
  loaded = m.loaded;
  should_be_loaded = m.should_be_loaded;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
module::module()
  : instance_id(0), loaded(false), should_be_loaded(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] m Object to copy.
 */
module::module(module const& m) : event(m) {
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
  event::operator=(m);
  _internal_copy(m);
  return (*this);
}

/**
 *  Get the type of this event (MODULE).
 *
 *  @return event::MODULE.
 */
int module::get_type() const {
  return (MODULE);
}
