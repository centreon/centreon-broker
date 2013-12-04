/*
** Copyright 2011-213 Merethis
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
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

/**
 *  Check that raw data is properly constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // Object.
  io::raw data;

  // Check construction.
  return ((data.type() != io::events::data_type<io::events::internal, 1>::value)
	  || (data.size() != 0));
}
