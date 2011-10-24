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
#include <QThread>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/processing/feeder.hh"
#include "test/processing/feeder/setable_endpoint.hh"
#include "test/processing/feeder/setable_stream.hh"

using namespace com::centreon::broker;

/**
 *  Run the feeder thread and check that it works properly.
 */
class wrapper : public QThread {
 private:
  QSharedPointer<setable_stream>
       _in;
  QSharedPointer<setable_stream>
       _out;
  bool _triggered;

 public:
  /**
   *  Default constructor.
   */
       wrapper(QSharedPointer<setable_stream> in,
               QSharedPointer<setable_stream> out)
    : _in(in), _out(out), _triggered(false) {}

  /**
   *  Destructor.
   */
       ~wrapper() {}

  /**
   *  Check if exception was triggered.
   *
   *  @return true if proper exception was triggered.
   */
  bool triggered() const {
    return (_triggered);
  }

  /**
   *  Thread entry point (check method).
   */
  void run() {
    processing::feeder f;
    f.prepare(
      _in.staticCast<io::stream>(),
      _out.staticCast<io::stream>());
    try {
      f.run();
    }
    catch (exceptions::with_pointer const& e) {
      _out->set_succeed(true);
      _out->write(e.ptr());
      _triggered = true;
    }
    return ;
  }
};

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
  QSharedPointer<setable_stream> ss1(new setable_stream);
  QSharedPointer<setable_stream> ss2(new setable_stream);
  ss1->set_succeed(true);
  ss2->set_succeed(true);
  ss2->store_events(true);

  // Wrapping object.
  wrapper w(ss1, ss2);

  // Launch check thread.
  w.start();

  // Wait some time.
  QTimer::singleShot(2000, &app, SLOT(quit()));
  app.exec();

  // Set failing flag.
  ss2->set_succeed(false);

  // Wait for thread termination.
  w.wait();

  // Check output content.
  int retval(0);
  unsigned int count(ss1->count());
  unsigned int i(0);
  for (QList<QSharedPointer<io::data> >::const_iterator
         it = ss2->events().begin(),
         end = ss2->events().end();
       it != end;
       ++it)
    if ((*it)->type() != "com::centreon::broker::io::raw")
      retval |= 1;
    else {
      QSharedPointer<io::raw> raw(it->staticCast<io::raw>());
      unsigned int val;
      memcpy(&val, raw->QByteArray::data(), sizeof(val));
      retval |= (val != ++i);
    }
  retval |= (i != count);
  retval |= !w.triggered();

  // Return check result.
  return (retval);
}
