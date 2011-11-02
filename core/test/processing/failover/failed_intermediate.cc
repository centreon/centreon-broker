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
#include <QSharedPointer>
#include <QTimer>
#include <unistd.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/feeder/setable_endpoint.hh"

using namespace com::centreon::broker;

/**
 *  Check that the failover class behave correctly if an intermediate
 *  failover fails.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Initialization.
  config::applier::init();

  // Application object.
  QCoreApplication app(argc, argv);

  // Enable logging.
  if (argc > 1)
    log_on_stderr();

  // First failover.
  QSharedPointer<setable_endpoint> endp1(new setable_endpoint);
  endp1->set_succeed(true);
  QSharedPointer<processing::failover> fo1(
    new processing::failover(true));
  fo1->set_endpoint(endp1.staticCast<io::endpoint>());

  // Second failover (intermediate).
  QSharedPointer<setable_endpoint> endp2(new setable_endpoint);
  endp2->set_succeed(false);
  QSharedPointer<processing::failover> fo2(
    new processing::failover(true));
  fo2->set_endpoint(endp2.staticCast<io::endpoint>());
  fo2->set_failover(fo1);
  fo2->set_retry_interval(1);

  // Last failover.
  QSharedPointer<setable_endpoint> endp3(new setable_endpoint);
  endp3->set_succeed(false);
  QSharedPointer<processing::failover> fo3(
    new processing::failover(true));
  fo3->set_endpoint(endp3.staticCast<io::endpoint>());
  fo3->set_failover(fo2);
  fo3->set_retry_interval(1);

  // Launch processing.
  QObject::connect(fo3.data(), SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(fo3.data(), SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(fo3.data(), SIGNAL(terminated()), &app, SLOT(quit()));
  fo3->start();
  app.exec();

  // Wait a while to get fo1 and fo2 launched because of failing
  // endpoints #2 and #3.
  QTimer::singleShot(2000, &app, SLOT(quit()));
  app.exec();

  // Enable endpoint #3.
  endp3->set_succeed(true);

  // Wait fo3 to reenable endpoint #3 and cancel fo2.
  QTimer::singleShot(2000, &app, SLOT(quit()));

  // Exit threads.
  fo3->process(false, false);
  fo3->wait();

  return (0);
}
