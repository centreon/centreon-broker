/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/config/logger.hh"

using namespace com::centreon::broker;

/**
 *  Check that default configuration for logging object matches
 *  expectations.
 *
 *  @return 0 on success.
 */
int main () {
  // Logger configuration.
  config::logger cfg;

  // Check content.
  return ((!cfg.config())
          || (cfg.debug())
          || (!cfg.error())
          || (cfg.info())
          || (cfg.level() != logging::HIGH)
          || (!cfg.name().isEmpty())
          || (cfg.type() != config::logger::unknown));
}
