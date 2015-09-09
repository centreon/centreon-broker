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
 *  Thread that will read a stream.
 */
class            concurrent_stream : public QThread {
public:
                 concurrent_stream(misc::shared_ptr<io::stream> stream)
    : _stream(stream), _failure(false) {}
                 ~concurrent_stream() {}

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
     try {
       while (true) {
         misc::shared_ptr<io::data> d;
         _stream->read(d);
       }
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

private:
  misc::shared_ptr<io::stream>
                 _stream;
  bool           _failure;
  unsigned short _port;
};

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
      while (!should_exit) {
        misc::shared_ptr<tcp::stream> stream =
          _acceptor->open().staticCast<tcp::stream>();

        if (!stream.isNull()) {
          stream->set_read_timeout(1);
          _streams.push_back(new concurrent_stream(stream));
          _streams.back()->start();
        }
      }
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
  std::vector<concurrent_stream*>
                 _streams;
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

  // Cleanup.
  config::applier::deinit();

  return (retval ? EXIT_FAILURE : EXIT_SUCCESS);
}
