/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/neb/check.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
check::check()
  : active_checks_enabled(false),
    check_type(0),
    host_id(0),
        next_check(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
check::check(check const& c) : io::data(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
check::~check() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
check& check::operator=(check const& c) {
  if (&c != this) {
    io::data::operator=(c);
    _internal_copy(c);
  }
  return (*this);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy internal data of the given object to the current instance.
 *
 *  @param[in] c Object to copy from.
 */
void check::_internal_copy(check const& c) {
  active_checks_enabled = c.active_checks_enabled;
  check_type = c.check_type;
  command_line = c.command_line;
  host_id = c.host_id;
    next_check = c.next_check;
  return ;
}
