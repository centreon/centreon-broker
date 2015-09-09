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
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
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
  multiplexing::engine::instance().start();

  // Log messages.
  if (argc > 1)
    log_on_stderr();

  // Endpoint.
  misc::shared_ptr<setable_endpoint> se(new setable_endpoint);
  se->set_succeed(true);

  // Subscriber.
  multiplexing::subscriber s("temporary_prefix_name");

  // Failover object.
  processing::failover f(se.staticCast<io::endpoint>(), false);

  // Launch thread.
  QObject::connect(&f, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&f, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&f, SIGNAL(terminated()), &app, SLOT(quit()));
  f.start();
  app.exec();

  // Wait some time.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Quit feeder thread.
  f.process(false, false);

  // Wait for thread termination.
  f.wait();

  // Check output content.
  s.process(false, false);
  int retval(se->streams().isEmpty());
  if (!retval) {
    misc::shared_ptr<setable_stream> ss(*se->streams().begin());
    unsigned int count(ss->get_count());
    unsigned int i(0);
    misc::shared_ptr<io::data> event;
    s.read(event, 0);
    while (!event.isNull()) {
      if (event->type() != io::events::data_type<io::events::internal, 1>::value)
        retval |= 1;
      else {
        misc::shared_ptr<io::raw> raw(event.staticCast<io::raw>());
        unsigned int val;
        memcpy(&val, raw->QByteArray::data(), sizeof(val));
        retval |= (val != ++i);
      }
      try {
        s.read(event, 0);
      }
      catch (io::exceptions::shutdown const& e) {
	event.clear();
      }
    }
    retval |= (i != count);
  }

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (retval);
}
