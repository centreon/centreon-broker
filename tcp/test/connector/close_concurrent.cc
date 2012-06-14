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
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "test/common.hh"

using namespace com::centreon::broker;

/**
 *  Thread that will connect TCP stream.
 */
class             concurrent : public QThread {
 private:
  tcp::connector* _conn;
  unsigned short  _port;

 public:
  /**
   *  Default constructor.
   */
                  concurrent() : _conn(NULL) {}

  /**
   *  Destructor.
   */
                  ~concurrent() {
    delete _conn;
  }

  /**
   *  Close acceptor.
   */
  void            close() {
    _conn->close();
    return ;
  }

  /**
   *  Thread entry point.
   */
  void            run() {
    // Wait a short time for the acceptor to be launched.
    {
      QMutex mutex;
      QMutexLocker lock(&mutex);
      QWaitCondition cv;
      cv.wait(&mutex, 400);
    }

    // Connect.
    _conn = new tcp::connector;
    _conn->connect_to("localhost", _port);

    // Will throw when socket is closed.
    try {
      misc::shared_ptr<io::stream> s(_conn->open());
      while (true)
        s->read();
    }
    catch (...) {}
    return ;
  }

  /**
   *  Set port to connect to.
   *
   *  @param[in] port Port to connect to.
   */
  void            set_port(unsigned short port) {
    _port = port;
    return ;
  }
};

/**
 *  Check that concurrent close works with TCP connector.
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

  // Port.
  unsigned short port(random_port());

  // Listener.
  tcp::acceptor a;
  a.listen_on(port);

  // Thread that will connect to local acceptor.
  concurrent c;
  c.set_port(port);
  c.start();

  {
    // Wait for connection.
    misc::shared_ptr<io::stream> s(a.open());

    // Concurrently close connector.
    c.close();
  }

  // Close acceptor.
  a.close();

  // For for thread to finish.
  return (!c.wait(2000));
}
