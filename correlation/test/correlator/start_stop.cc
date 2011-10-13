/*
** Copyright 2011 Merethis
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

#include <QList>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/correlation/correlator.hh"
#include "test/correlator/common.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

/**
 *  Check that node is properly default constructed.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();

  // Create correlator.
  correlator c;

  // Start correlator then stop it.
  c.starting();
  c.stopping();

  // Check correlation content.
  QList<QSharedPointer<io::data> > content;
  add_engine_state(content, true);
  add_engine_state(content, false);
  return (!check_content(c, content));
}
