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

#include <QCoreApplication>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/feeder/setable_endpoint.hh"

using namespace com::centreon::broker;

/**
 *  Check that Qt event loop break properly.
 *
 *  @param[in] argc Arguments count.
 *  @param[in] argv Arguments values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Initialization.
  config::applier::init();

  // Qt core application.
  QCoreApplication app(argc, argv);

  // Enable logging.
  log_on_stderr();

  // Endpoint.
  QSharedPointer<setable_endpoint> se(new setable_endpoint);
  se->set_succeed(false);

  // Failover object.
  processing::failover f(true);
  f.set_endpoint(se);
  f.set_retry_interval(20);

  // Launch thread.
  f.start();

  // Wait some time.
  QTimer::singleShot(2000, &app, SLOT(quit()));
  app.exec();

  // Thread should be in Qt event loop, waiting its timeout to elapse.
  // This should break it.
  f.process(false, false);

  // Wait for quick thread termination.
  int retval;
  if (f.wait(3))
    retval = 0;
  else {
    // No quick exit, wait thread.
    retval = 1;
    f.wait();
  }

  // Return check result.
  return (retval);
}
