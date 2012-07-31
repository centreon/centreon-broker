/*
** Copyright 2011-2012 Merethis
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

#include <cmath>
#include "com/centreon/broker/storage/perfdata.hh"

using namespace com::centreon::broker;

/**
 *  Check that the perfdata object properly default constructs.
 *
 *  @return 0 on success.
 */
int main() {
  // Build object.
  storage::perfdata p;

  // Check properties values.
  return (!isnan(p.critical())
          || !isnan(p.max())
          || !isnan(p.min())
          || !p.name().isEmpty()
          || !p.unit().isEmpty()
          || !isnan(p.value())
          || (p.value_type() != storage::perfdata::gauge)
          || !isnan(p.warning()));
}
