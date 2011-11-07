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
 *  Check that the failover class behave correctly if a failover
 *  directly recovers.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 */
int main(int argc, char* argv[]) {
  // Application object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  // Enable logging.
  if (argc > 1)
    log_on_stderr();

  // First failover.
  QSharedPointer<setable_endpoint> endp1(new setable_endpoint);
  endp1->set_succeed(true);
  QSharedPointer<processing::failover> fo1(
    new processing::failover(true));
  fo1->set_endpoint(endp1.staticCast<io::endpoint>());

  // Second failover.
  QSharedPointer<setable_endpoint> endp2(new setable_endpoint);
  endp2->set_succeed(true);
  processing::failover fo2(true);
  fo2.set_endpoint(endp2.staticCast<io::endpoint>());
  fo2.set_failover(fo1);
  fo2.set_retry_interval(1);

  // Launch threads.
  fo2.start();
  QTimer::singleShot(500, &app, SLOT(quit()));
  app.exec();

  // Failover thread fails.
  int retval(0);
  if (endp1->opened_streams() != 1)
    retval = 1;
  else {
    (*endp1->streams().begin())->process(true, false);
    QTimer::singleShot(1500, &app, SLOT(quit()));
    app.exec();
  }

  // Thread must be running.
  retval |= !fo2.isRunning();

  // Exit threads.
  fo2.process(false, false);
  fo2.wait();

  // Only one open stream means success.
  retval |= ((endp1->opened_streams() != 1)
             || (endp2->opened_streams() != 1));

  // Return check result.
  return (retval);
}
