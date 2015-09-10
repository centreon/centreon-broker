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
  misc::shared_ptr<setable_endpoint> endp1(new setable_endpoint);
  endp1->set_succeed(true);
  misc::shared_ptr<processing::failover>
    fo1(new processing::failover(
                          endp1.staticCast<io::endpoint>(),
                          true,
                          "failover1"));

  // Second failover.
  misc::shared_ptr<setable_endpoint> endp2(new setable_endpoint);
  endp2->set_succeed(true);
  processing::failover fo2(
                         endp2.staticCast<io::endpoint>(),
                         true,
                         "failover2");
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

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (retval);
}
