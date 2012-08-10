/*
** Copyright 2012 Merethis
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

#include "com/centreon/broker/neb/module.hh"

using namespace com::centreon::broker;

/**
 *  Check module's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::module mod;

  // Check.
  return ((mod.args != "")
          || (mod.enabled != true)
          || (mod.filename != "")
          || (mod.instance_id != 0)
          || (mod.loaded != false)
          || (mod.should_be_loaded != false)
          || (mod.type()
              != "com::centreon::broker::neb::module"));
}
