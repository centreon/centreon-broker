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
#include <QObject>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/feeder/setable_endpoint.hh"

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
  processing::failover f(false);
  f.set_endpoint(se);
  f.set_retry_interval(1);

  // Launch thread.
  QObject::connect(&f, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&f, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&f, SIGNAL(terminated()), &app, SLOT(quit()));
  f.start();
  app.exec();

  // Wait some time.
  QTimer::singleShot(3400, &app, SLOT(quit()));
  app.exec();

  // Quit feeder thread.
  f.process(false, false);

  // Wait for thread termination.
  f.wait();

  // The number of stream is the number of retry.
  return ((se->opened_streams() != 4) || (f.get_retry_interval() != 1));
}
