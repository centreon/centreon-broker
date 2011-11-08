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
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/local/acceptor.hh"
#include "com/centreon/broker/local/connector.hh"

using namespace com::centreon::broker;

#define TEMP_FILE "/tmp/centreon_broker_unit_test"

/**
 *  Thread that will connect local stream.
 */
class               concurrent : public QThread {
 private:
  local::connector* _conn;
  QString           _path;

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
  void              close() {
    _conn->close();
    return ;
  }

  /**
   *  Thread entry point.
   */
  void              run() {
    // Wait a short time for the acceptor to be launched.
    {
      QMutex mutex;
      QMutexLocker lock(&mutex);
      QWaitCondition cv;
      cv.wait(&mutex, 400);
    }

    // Connect.
    _conn = new local::connector;
    _conn->connect_to(_path);

    // Will throw when socket is closed.
    try {
      QSharedPointer<io::stream> s(_conn->open());
      while (true)
        s->read();
    }
    catch (...) {}
    return ;
  }

  /**
   *  Set the socket to connect to.
   *
   *  @param[in] path Socket to connect to.
   */
  void              set_path(QString const& path) {
    _path = path;
    return ;
  }
};

/**
 *  Check that concurrent close works with local connector.
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

  // Listener.
  local::acceptor a;
  a.listen_on(TEMP_FILE);

  // Thread that will connect to local acceptor.
  concurrent c;
  c.set_path(TEMP_FILE);
  c.start();

  {
    // Wait for connection.
    QSharedPointer<io::stream> s(a.open());

    // Concurrently close connector.
    c.close();
  }

  // Close acceptor.
  a.close();

  // For for thread to finish.
  return (!c.wait(2000));
}
