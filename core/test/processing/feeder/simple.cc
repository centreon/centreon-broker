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
#include "com/centreon/broker/io/events.hh"
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
  // Qt core application.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

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
    if ((*it)->type() != io::events::data_type<io::events::internal, 1>::value)
      retval |= 1;
    else {
      misc::shared_ptr<io::raw> raw(it->staticCast<io::raw>());
      unsigned int val;
      memcpy(&val, raw->QByteArray::data(), sizeof(val));
      retval |= (val != ++i);
    }
  retval |= (i != count);

  // Cleanup.
  config::applier::deinit();

  // Return check result.
  return (retval);
}
