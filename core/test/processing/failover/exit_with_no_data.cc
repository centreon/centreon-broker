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
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/processing/failover.hh"
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

  // Qt core application.
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

  // Endpoint.
  QSharedPointer<setable_endpoint> se1(new setable_endpoint);
  se1->set(true);

  // Failover object.
  processing::failover f1(true);
  f1.set_endpoint(se1);

  // Launch thread.
  f1.start();

  // Wait some time.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Quit failover thread.
  f1.process(false, true);

  // Wait for thread termination.
  f1.wait();

  // Failover thread.
  QSharedPointer<setable_endpoint> se2(new setable_endpoint);
  se2->set(true);
  QSharedPointer<processing::failover>
    f2(new processing::failover(true));
  f2->set_endpoint(se2);
  se1->set(true);
  f1.process(true, true);
  f1.set_failover(f2);
  f1.start();

  // Some processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Failover thread #2 has finished processing.
  se2->set(false);

  // Some additionnal processing.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Exit threads.
  f1.process(false, true);
  f1.wait();

  // Return check result.
  return (0);
}
