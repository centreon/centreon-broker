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
#include <QThread>
#include <QTimer>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "test/common.hh"

using namespace com::centreon::broker;

/**
 *  Thread that will listen on a port.
 */
class            concurrent : public QThread {
 private:
  tcp::acceptor* _acceptor;
  unsigned short _port;

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
  void           close() {
    _acceptor->close();
    return ;
  }

  /**
   *  Thread entry point.
   */
  void           run() {
    _acceptor = new tcp::acceptor;
    _acceptor->listen_on(_port);
    try {
      _acceptor->open();
    }
    catch (...) {}
    return ;
  }

  /**
   *  Set port to listen on.
   *
   *  @param[in] port Port to listen on.
   */
  void           set_port(unsigned short port) {
    _port = port;
    return ;
  }
};

/**
 *  Check that concurrent close works with TCP acceptor.
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
  c.set_port(random_port());
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
