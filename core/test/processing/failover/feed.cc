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
  // Initialization.
  config::applier::init();
  multiplexing::engine::instance().start();

  // Qt core application.
  QCoreApplication app(argc, argv);

  // Log messages.
  if (argc > 1)
    log_on_stderr();

  // Endpoint.
  QSharedPointer<setable_endpoint> se(new setable_endpoint);
  se->set_succeed(true);

  // Subscriber.
  multiplexing::subscriber s;

  // Failover object.
  processing::failover f(false);
  f.set_endpoint(se);

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
    QSharedPointer<setable_stream> ss(*se->streams().begin());
    unsigned int count(ss->get_count());
    unsigned int i(0);
    QSharedPointer<io::data> event(s.read(0));
    while (!event.isNull()) {
      if (event->type() != "com::centreon::broker::io::raw")
        retval |= 1;
      else {
        QSharedPointer<io::raw> raw(event.staticCast<io::raw>());
        unsigned int val;
        memcpy(&val, raw->QByteArray::data(), sizeof(val));
        retval |= (val != ++i);
      }
      try {
        event = s.read(0);
      }
      catch (io::exceptions::shutdown const& e) {
	event.clear();
      }
    }
    retval |= (i != count);
  }

  // Return check result.
  return (retval);
}
