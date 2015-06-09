/*
** Copyright 2011-2013,2015 Merethis
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
#include <QObject>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/failover/setable_endpoint.hh"

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
  // Qt core application.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  // Enable logging.
  if (argc > 1)
    log_on_stderr();

  // Endpoint.
  misc::shared_ptr<setable_endpoint> se(new setable_endpoint);
  se->set_succeed(false);

  // Subscriber.
  misc::shared_ptr<multiplexing::subscriber>
    s(new multiplexing::subscriber(
                          "processing_failover_loop_break",
                          ""));

  // Failover object.
  processing::failover f(
                         se.staticCast<io::endpoint>(),
                         s,
                         "processing_failover_loop_break",
                         "");
  f.set_retry_interval(20);

  // Launch thread.
  f.start();

  // Wait some time.
  QTimer::singleShot(5000, &app, SLOT(quit()));
  app.exec();

  // Thread should be in Qt event loop, waiting its timeout to elapse.
  // This should break it.
  f.exit();

  // Wait for quick thread termination.
  int retval;
  if (f.wait(3))
    retval = 0;
  else {
    // No quick exit, wait thread.
    retval = 1;
    f.wait();
  }

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (retval);
}
