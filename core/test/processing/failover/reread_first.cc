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
#include <unistd.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/processing/failover.hh"
#include "test/processing/feeder/common.hh"
#include "test/processing/feeder/setable_endpoint.hh"

using namespace com::centreon::broker;

#define MSG "some random message for Centreon Broker"

/**
 *  Check that failover events are read before normal events.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Initialization.
  config::applier::init();
  multiplexing::engine::instance().start();

  // Application object.
  QCoreApplication app(argc, argv);

  // Enable logging.
  if (argc > 1)
    log_on_stderr();

  // First failover.
  misc::shared_ptr<setable_endpoint> endp1(new setable_endpoint);
  endp1->set_succeed(true);
  endp1->set_initial_replay_events(true);
  misc::shared_ptr<processing::failover>
    fo1(new processing::failover(true));
  fo1->set_name("failover1");
  fo1->set_endpoint(endp1.staticCast<io::endpoint>());

  // Second failover.
  misc::shared_ptr<setable_endpoint> endp2(new setable_endpoint);
  endp2->set_succeed(true);
  endp2->set_initial_store_events(true);
  misc::shared_ptr<processing::failover>
    fo2(new processing::failover(true));
  fo1->set_name("failover2");
  fo2->set_endpoint(endp2.staticCast<io::endpoint>());
  fo2->set_failover(fo1);

  // Publish an event that should be the last event processed by fo1.
  {
    misc::shared_ptr<io::raw> r(new io::raw);
    r->append(MSG);
    multiplexing::engine::instance().publish(r.staticCast<io::data>());
  }

  // Launch processing.
  QObject::connect(fo2.data(), SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(fo2.data(), SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(fo2.data(), SIGNAL(terminated()), &app, SLOT(quit()));
  fo2->start();
  app.exec();

  // Some processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Failover thread #1 has finished processing.
  endp1->set_process(false, true);

  // Some additionnal processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Exit threads.
  fo2->process(false, false);
  fo2->wait();

  // Check stream content.
  int retval(0);
  if (endp1->streams().isEmpty() || endp2->streams().isEmpty()) {
    logging::error(logging::high) << "test: no stream for one endpoint";
    retval = 1;
  }
  else {
    // Check automatically generated events.
    misc::shared_ptr<setable_stream> ss1(*endp1->streams().begin());
    misc::shared_ptr<setable_stream> ss2(*endp2->streams().begin());
    unsigned int count(ss1->get_count());
    unsigned int i(0);
    logging::info(logging::high) << "test: stream1 has " << count
      << " events, stream2 has " << ss2->get_stored_events().size()
      << " events";
    for (QList<misc::shared_ptr<io::data> >::const_iterator
           it(ss2->get_stored_events().begin()),
           end(ss2->get_stored_events().end());
         (i < count) && (it != end);
         ++it) {
      if ((*it)->type() != "com::centreon::broker::io::raw") {
        logging::error(logging::high)
          << "test: read data which is not raw";
        retval |= 1;
      }
      else {
        misc::shared_ptr<io::raw> raw(it->staticCast<io::raw>());
        unsigned int val;
        memcpy(&val, raw->QByteArray::data(), sizeof(val));
        retval |= (val != ++i);
        if (retval)
          logging::error(logging::high)
            << "test: read data that does not match expectations";
      }
    }
    retval |= (i != count);
    if (retval)
      logging::error(logging::high) << "test: invalid event count (got "
        << i << ", expected " << count << ")";

    // Check event list size.
    retval |= (ss2->get_stored_events().size()
               != static_cast<int>(count + 1));
    if (retval)
      logging::error(logging::high)
        << "test: invalid stored event size (got "
        << ss2->get_stored_events().size()
        << ", expected " << (count + 1) << ")";

    // Check first published event.
    misc::shared_ptr<io::data> d(ss2->get_stored_events().last());
    if (d.isNull()
        || (d->type() != "com::centreon::broker::io::raw")) {
      logging::error(logging::high)
        << "test: null or invalid last item";
      retval |= 1;
    }
    else {
      misc::shared_ptr<io::raw> raw(d.staticCast<io::raw>());
      retval |= strncmp(
        raw->QByteArray::data(),
        MSG,
        sizeof(MSG) - 1);
      if (retval)
        logging::error(logging::high)
          << "test: invalid last event's content";
    }
  }

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
