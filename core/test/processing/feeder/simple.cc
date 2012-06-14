/*
** Copyright 2011-2012 Merethis
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
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/processing/feeder.hh"
#include "test/processing/feeder/setable_endpoint.hh"
#include "test/processing/feeder/setable_stream.hh"

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

  // Streams.
  misc::shared_ptr<setable_stream> ss1(new setable_stream);
  misc::shared_ptr<setable_stream> ss2(new setable_stream);
  ss1->process(true, true);
  ss2->process(true, true);
  ss2->set_store_events(true);

  // Feeder object.
  processing::feeder f;
  f.prepare(ss1.staticCast<io::stream>(), ss2.staticCast<io::stream>());

  // Launch feeder.
  QObject::connect(&f, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&f, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&f, SIGNAL(terminated()), &app, SLOT(quit()));
  f.start();

  // Wait some time.
  app.exec();
  QTimer::singleShot(200, &app, SLOT(quit()));
  app.exec();

  // Quit feeder thread.
  f.exit();

  // Wait for thread termination.
  f.wait();

  // Check output content.
  int retval(0);
  unsigned int count(ss1->get_count());
  unsigned int i(0);
  for (QList<misc::shared_ptr<io::data> >::const_iterator
         it(ss2->get_stored_events().begin()),
         end(ss2->get_stored_events().end());
       it != end;
       ++it)
    if ((*it)->type() != "com::centreon::broker::io::raw")
      retval |= 1;
    else {
      misc::shared_ptr<io::raw> raw(it->staticCast<io::raw>());
      unsigned int val;
      memcpy(&val, raw->QByteArray::data(), sizeof(val));
      retval |= (val != ++i);
    }
  retval |= (i != count);

  // Return check result.
  return (retval);
}
