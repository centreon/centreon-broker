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

  // Failover object.
  processing::failover f(se.staticCast<io::endpoint>(), false);
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
  int retval((se->opened_streams() != 4)
             || (f.get_retry_interval() != 1));

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
