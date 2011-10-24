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
#include <QSharedPointer>
#include <QTimer>
#include <unistd.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/processing/failover.hh"
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
  logging::file::with_thread_id(true);
  logging::file::with_timestamp(true);
  config::logger log_obj;
  log_obj.config(true);
  log_obj.debug(true);
  log_obj.error(true);
  log_obj.info(true);
  log_obj.level(logging::low);
  log_obj.type(config::logger::standard);
  log_obj.name("stderr");
  QList<config::logger> loggers;
  loggers.push_back(log_obj);
  config::applier::logger::instance().apply(loggers);

  // First failover.
  QSharedPointer<setable_endpoint> endp1(new setable_endpoint);
  endp1->set(true);
  endp1->save_streams(true);
  QSharedPointer<processing::failover> fo1(
    new processing::failover(true));
  fo1->set_endpoint(endp1.staticCast<io::endpoint>());

  // Second failover.
  QSharedPointer<setable_endpoint> endp2(new setable_endpoint);
  endp2->set(true);
  endp2->set_initial_store_events(true);
  endp2->save_streams(true);
  QSharedPointer<processing::failover> fo2(
    new processing::failover(true));
  fo2->set_endpoint(endp2.staticCast<io::endpoint>());
  fo2->set_failover(fo1);

  // Publish an event that should be the last event processed by fo1.
  {
    QSharedPointer<io::raw> r(new io::raw);
    r->append(MSG);
    multiplexing::engine::instance().publish(r.staticCast<io::data>());
  }

  // Launch processing.
  fo2->start();

  // Some processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Failover thread #1 has finished processing.
  endp1->set(false);

  // Some additionnal processing.
  QTimer::singleShot(500, &app, SLOT(quit()));
  app.exec();

  // Exit threads.
  fo2->process(false, true);
  fo2->wait();

  // Check stream content.
  int retval(0);
  if (endp1->streams().isEmpty() || endp2->streams().isEmpty())
    retval = 1;
  else {
    // Check automatically generated events.
    QSharedPointer<setable_stream> ss1(*endp1->streams().begin());
    QSharedPointer<setable_stream> ss2(*endp2->streams().begin());
    unsigned int count(ss1->count());
    unsigned int i(0);
    for (QList<QSharedPointer<io::data> >::const_iterator
           it = ss2->events().begin(),
           end = ss2->events().end();
         (i < count) && (it != end);
         ++it) {
      if ((*it)->type() != "com::centreon::broker::io::raw")
        retval |= 1;
      else {
        QSharedPointer<io::raw> raw(it->staticCast<io::raw>());
        unsigned int val;
        memcpy(&val, raw->QByteArray::data(), sizeof(val));
        retval |= (val != ++i);
      }
    }
    retval |= (i != count);

    // Check event list size.
    retval |= (ss2->events().size() != (count + 1));

    // Check first published event.
  }

  return (retval);
}
