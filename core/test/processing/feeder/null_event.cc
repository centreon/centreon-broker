/*
** Copyright 2015 Merethis
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

#include <cstdlib>
#include <QCoreApplication>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/processing/feeder.hh"
#include "test/processing/feeder/setable_stream.hh"

using namespace com::centreon::broker;

/**
 *  Check that feeder thread accepts null events.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Qt core application.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();
  multiplexing::engine::instance().start();

  // Streams.
  misc::shared_ptr<multiplexing::subscriber>
    in(new multiplexing::subscriber("unittest"));
  misc::shared_ptr<setable_stream>
    out(new setable_stream);
  out->process(true, true);

  // Feeder object.
  processing::feeder f;
  f.prepare("unit test", in, out);

  // Launch feeder.
  f.start();

  // Wait some time.
  QTimer::singleShot(2000, &app, SLOT(quit()));
  app.exec();

  // Write null event.
  multiplexing::publisher p;
  p.write(misc::shared_ptr<io::data>());

  // Wait some time.
  QTimer::singleShot(3000, &app, SLOT(quit()));
  app.exec();

  // Thread must be running.
  int retval(f.wait(0) ? EXIT_FAILURE : EXIT_SUCCESS);

  // Quit feeder thread.
  f.exit();
  p.write(misc::shared_ptr<io::data>());
  f.wait();

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (retval);
}
