/*
** Copyright 2012-2013 Merethis
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

#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;

/**
 *  Check instance's default constructor.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  neb::instance i;

  // Check.
  return ((i.engine != "")
          || (i.id != 0)
          || (i.is_running != true)
          || (i.name != "")
          || (i.pid != 0)
          || (i.program_end != 0)
          || (i.program_start != 0)
          || (i.version != "")
          || (i.type()
              != io::data::data_type(io::data::neb, neb::de_instance)));
}
