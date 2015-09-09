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

#include <QCoreApplication>
#include <QTcpSocket>
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
      misc::shared_ptr<tcp::stream>
        s(_acceptor->open().staticCast<tcp::stream>());
      if (!s.isNull()) {
        s->set_read_timeout(1);
        while (true) {
          misc::shared_ptr<io::data> d;
          s->read(d);
        }
      }
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
 *  Check that close works with open stream.
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
  unsigned short port(random_port());
  c.set_port(port);
  QObject::connect(&c, SIGNAL(finished()), &app, SLOT(quit()));
  QObject::connect(&c, SIGNAL(started()), &app, SLOT(quit()));
  QObject::connect(&c, SIGNAL(terminated()), &app, SLOT(quit()));
  c.start();
  app.exec();

  // Wait for thread to listen.
  QTimer::singleShot(500, &app, SLOT(quit()));
  app.exec();

  // Open new stream.
  int retval(0);
  QTcpSocket sock;
  sock.connectToHost("localhost", port);
  if (!sock.waitForConnected())
    retval = 1;

  // Concurrently close acceptor.
  c.close();

  // For for thread to finish.
  retval = (!c.wait(5000) || retval);

  // Cleanup.
  config::applier::deinit();

  return (retval);
}
