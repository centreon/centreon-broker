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

#include <cstdlib>
#include <iostream>
#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#ifndef _WIN32
#  include <unistd.h>
#endif // !_WIN32
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "test/common.hh"

using namespace com::centreon::broker;

// Exit flag.
static volatile bool should_exit(false);

/**
 *  Thread that will listen on a port.
 */
class            concurrent_acceptor : public QThread {
public:
  /**
   *  Default constructor.
   */
                 concurrent_acceptor()
    : _acceptor(NULL), _failure(false) {}

  /**
   *  Destructor.
   */
                 ~concurrent_acceptor() {
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
   *  Check if thread failed.
   *
   *  @return true if thread encountered a failure.
   */
  bool           failure() const {
    return (_failure);
  }

  /**
   *  Thread entry point.
   */
  void           run() {
    _acceptor = new tcp::acceptor;
    _acceptor->listen_on(_port);
    try {
      while (!should_exit)
        _acceptor->open();
    }
    catch (io::exceptions::shutdown const& e) {
      (void)e;
    }
    catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      _failure = true;
    }
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

private:
  tcp::acceptor* _acceptor;
  bool           _failure;
  unsigned short _port;
};

/**
 *  Thread that will connect to the current host on a port.
 */
class             concurrent_connector : public QThread {
public:
  /**
   *  Default constructor.
   */
                  concurrent_connector()
    : _connector(NULL), _failure(false) {}

  /**
   *  Destructor.
   */
                  ~concurrent_connector() {
    delete _connector;
  }

  /**
   *  Check if thread encountered a failure.
   *
   *  @return true if a failure occured.
   */
  bool            failure() const {
    return (_failure);
  }

  /**
   *  Thread entry point.
   */
  void            run() {
    _connector = new tcp::connector;
    _connector->connect_to("localhost", _port);
    misc::shared_ptr<io::stream> s;
    try {
      while (!s.data() && !should_exit)
        s = _connector->open();
    }
    catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
      _failure = true;
    }
    if (s.data()) {
      try {
        while (1) {
          misc::shared_ptr<io::data> d;
          s->read(d);
        }
      }
      catch (...) {}
    }
    else {
      std::cerr << "connector " << this
                << " could not open connection" << std::endl;
      _failure = true;
    }
    return ;
  }

  /**
   *  Set the port to connect on.
   *
   *  @param[in] port Port to connect on.
   */
  void            set_port(unsigned short port) {
    _port = port;
    return ;
  }

private:
  tcp::connector* _connector;
  bool            _failure;
  unsigned short  _port;
};

/**
 *  Check that concurrent close works with TCP acceptor.
 *
 *  @param[in] argc Argument count.
 *  @param[in] argv Argument values.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main(int argc, char* argv[]) {
  // Qt core object.
  QCoreApplication app(argc, argv);

  // Initialization.
  config::applier::init();
#ifndef _WIN32
  srand(getpid());
#endif // !_WIN32

  // Random port.
  unsigned short port(random_port());
  std::cerr << "PORT " << port << std::endl;

  // Thread that will listen on a port.
  concurrent_acceptor accptr;
  accptr.set_port(port);
  QObject::connect(&accptr, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&accptr, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&accptr, SIGNAL(terminated()), &app, SLOT(quit()));
  accptr.start();
  app.exec();

  // Wait for thread to listen.
  QTimer::singleShot(500, &app, SLOT(quit()));
  app.exec();

  // Launch connections.
  concurrent_connector cnnctrs[5];
  for (unsigned int i(0); i < sizeof(cnnctrs) / sizeof(*cnnctrs); ++i) {
    cnnctrs[i].set_port(port);
    cnnctrs[i].start();
  }

  // Wait for threads to connect.
  QTimer::singleShot(1000, &app, SLOT(quit()));
  app.exec();

  // Concurrently close acceptor.
  should_exit = true;
  accptr.close();

  // For for connecting threads to finish.
  int retval(0);
  for (unsigned int i(0); i < sizeof(cnnctrs) / sizeof(*cnnctrs); ++i) {
    retval |= !cnnctrs[i].wait(1000);
    retval |= cnnctrs[i].failure();
  }

  // Wait for acceptor thread to finish.
  retval |= !accptr.wait(2000);
  retval |= accptr.failure();

  return (retval ? EXIT_FAILURE : EXIT_SUCCESS);
}
