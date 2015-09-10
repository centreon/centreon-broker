/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <QCoreApplication>
#include <QObject>
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
  // Application object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  // Enable logging.
  if (argc > 1)
    log_on_stderr();

  // First failover.
  misc::shared_ptr<setable_endpoint> endp1(new setable_endpoint);
  endp1->set_succeed(true);
  misc::shared_ptr<processing::failover>
    fo1(new processing::failover(
                          endp1.staticCast<io::endpoint>(),
                          true));

  // Second failover (intermediate).
  misc::shared_ptr<setable_endpoint> endp2(new setable_endpoint);
  endp2->set_succeed(false);
  misc::shared_ptr<processing::failover>
    fo2(new processing::failover(
                          endp2.staticCast<io::endpoint>(),
                          true));
  fo2->set_failover(fo1);
  fo2->set_retry_interval(1);

  // Last failover.
  misc::shared_ptr<setable_endpoint> endp3(new setable_endpoint);
  endp3->set_succeed(false);
  misc::shared_ptr<processing::failover>
    fo3(new processing::failover(
                          endp3.staticCast<io::endpoint>(),
                          true));
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

  // Cleanup.
  config::applier::deinit();

  return (0);
}
