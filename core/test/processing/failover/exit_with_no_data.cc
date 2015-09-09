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
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/feeder/setable_endpoint.hh"

using namespace com::centreon::broker;

/**
 *  Check that failover thread exits even if no data is available.
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
  misc::shared_ptr<setable_endpoint> se1(new setable_endpoint);
  se1->set_succeed(true);

  // Failover object.
  processing::failover f1(se1.staticCast<io::endpoint>(), true);

  // Launch thread.
  f1.start();

  // Wait some time.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Quit failover thread.
  f1.process(false, false);

  // Wait for thread termination.
  f1.wait();

  // Failover thread.
  misc::shared_ptr<setable_endpoint> se2(new setable_endpoint);
  se2->set_succeed(true);
  misc::shared_ptr<processing::failover>
    f2(new processing::failover(se2.staticCast<io::endpoint>(), true));
  se1->set_succeed(true);
  f1.set_failover(f2);
  f1.start();

  // Some processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Failover thread #2 has finished processing.
  se2->set_succeed(false);

  // Some additionnal processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Exit threads.
  f1.process(false, false);
  f1.wait();

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (0);
}
