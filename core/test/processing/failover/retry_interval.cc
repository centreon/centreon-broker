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
 *  Check that simple event feeding works properly.
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

  // Subscriber.
  misc::shared_ptr<multiplexing::subscriber>
    s(new multiplexing::subscriber(
                          "processing_failover_retry_interval",
                          ""));

  // Endpoint.
  misc::shared_ptr<setable_endpoint> se(new setable_endpoint);
  se->set_succeed(false);

  // Failover object.
  processing::failover f(
                         se.staticCast<io::endpoint>(),
                         s,
                         "processing_failover_retry_interval",
                         "");
  f.set_retry_interval(1);

  // Launch thread.
  f.start();

  // Wait some time.
  QTimer::singleShot(6000, &app, SLOT(quit()));
  app.exec();

  // Quit feeder thread.
  f.exit();

  // Wait for thread termination.
  f.wait();

  // The number of stream is the number of retry.
  int retval((se->opened_streams() != 4)
             || (f.get_retry_interval() != 1));

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
