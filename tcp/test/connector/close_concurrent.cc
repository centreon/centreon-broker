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

#include <cstdlib>
#include <QCoreApplication>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#ifndef _WIN32
#  include <unistd.h>
#endif // !_WIN32
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
      misc::shared_ptr<tcp::stream> s(_conn->open().staticCast<tcp::stream>());
      s->set_read_timeout(1);
      while (true) {
        misc::shared_ptr<io::data> d;
        s->read(d);
      }
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
#ifndef _WIN32
  srand(getpid());
#endif // !_WIN32

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
  bool error(!c.wait(2000));

  // Cleanup.
  config::applier::deinit();

  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
}
