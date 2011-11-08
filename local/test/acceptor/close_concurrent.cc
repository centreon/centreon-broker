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
#include "com/centreon/broker/local/acceptor.hh"

using namespace com::centreon::broker;

#define TEMP_FILE "/tmp/centreon_broker_unit_test"

/**
 *  Thread that will listen on a port.
 */
class              concurrent : public QThread {
 private:
  local::acceptor* _acceptor;
  QString          _path;

 public:
  /**
   *  Default constructor.
   */
                   concurrent() : _acceptor(NULL) {}

  /**
   *  Destructor.
   */
                   ~concurrent() {
    delete _acceptor;
  }

  /**
   *  Close acceptor.
   */
  void             close() {
    _acceptor->close();
    return ;
  }

  /**
   *  Thread entry point.
   */
  void             run() {
    _acceptor = new local::acceptor;
    _acceptor->listen_on(_path);
    try {
      _acceptor->open();
    }
    catch (...) {}
    return ;
  }

  /**
   *  Set socket to listen on.
   *
   *  @param[in] path Socket to listen on.
   */
  void           set_path(QString const& path) {
    _path = path;
    return ;
  }
};

/**
 *  Check that concurrent close works with local acceptor.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return 0 on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();

  // Thread that will listen on a port.
  concurrent c;
  c.set_path(TEMP_FILE);
  QObject::connect(&c, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&c, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&c, SIGNAL(terminated()), &app, SLOT(quit()));
  c.start();
  app.exec();

  // Wait for thread to listen.
  QTimer::singleShot(500, &app, SLOT(quit()));
  app.exec();

  // Concurrently close acceptor.
  c.close();

  // For for thread to finish.
  return (!c.wait(2000));
}
