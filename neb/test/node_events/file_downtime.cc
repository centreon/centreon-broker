/*
** Copyright 2011-2015 Centreon
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

#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <QMap>
#include <QPair>
#include <QThread>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/persistent_cache.hh"
#include "common.hh"
#include "vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Get a file.
 *
 *  @param[in] path  Path of the file.
 *
 *  @return          A string containing the file.
 */
std::string get_file(std::string const& path) {
  std::ostringstream oss;
  std::fstream fs;

  fs.exceptions(std::fstream::failbit | std::fstream::badbit);
  fs.open(
       path.c_str());
  oss << fs.rdbuf();
  return (oss.str());
}

/**
 *  Write a file.
 *
 *  @param[in] path   Path of the file.
 *  @param[in] value  The value of the file.
 */
void write_file(std::string const& path, std::string const& value) {
  std::fstream fs;

  fs.exceptions(std::fstream::failbit | std::fstream::badbit);
  fs.open(
      path.c_str(),
      std::fstream::in | std::fstream::out | std::fstream::trunc);
  fs.write(value.c_str(), value.size());
  fs.close();
}

/**
 *  Check that node events fixed downtime properly work.
 *
 *  @return EXIT_SUCCESS on success.
 */
int main() {
  // Return value.
  int retval(EXIT_FAILURE);

  std::string persistent_cache_file(::tmpnam(NULL));
  std::string temporary_config_file(::tmpnam(NULL));

  // Initialization.
  config::applier::init();
  multiplexing::engine::load();
  // Start the multiplexing engine.
  test_stream t;
  multiplexing::engine::instance().hook(t);
  multiplexing::engine::instance().start();

  try {
    misc::shared_ptr<persistent_cache> cache(
      new persistent_cache(persistent_cache_file));

    {
    // Create node event stream.
      node_events_stream test(
        "1",
        cache,
        PROJECT_SOURCE_DIR "/neb/test/node_events/cfg/downtime.cfg");

      // Send initial service status.
      {
        misc::shared_ptr<neb::service> sst(new neb::service);
        sst->host_id = 42;
        sst->service_id = 24;
        sst->last_hard_state = 0;
        sst->last_hard_state_change = 123456789;
        sst->host_name = "42";
        sst->service_description = "24";
        test.write(sst);
      }
      {
        misc::shared_ptr<neb::service_status> ss(new neb::service_status);
        ss->host_id = 42;
        ss->service_id = 24;
        ss->last_hard_state = 0;
        ss->last_hard_state_change = 123456789;
        test.write(ss);
      }
    }

    QString config_file = QString::fromStdString(get_file(
      PROJECT_SOURCE_DIR "/neb/test/node_events/cfg/file_downtime.cfg"));
    time_t now = ::time(NULL);
    QString ts = QString::number((unsigned long)now);
    QString ts2 = QString::number((unsigned long)now + 3);
    config_file.replace("$TIMESTAMP$", ts);
    config_file.replace("$TIMESTAMP2$", ts2);
    write_file(temporary_config_file, config_file.toStdString());

    // Create node event stream.
    node_events_stream test(
      "2",
      cache,
      temporary_config_file);

    ::sleep(5);

    // Check content.
    multiplexing::engine::instance().stop();
    t.finalize();

    QList<misc::shared_ptr<io::data> > content;
    add_downtime(content, now, now + 3, 3, true, 42, 24, 1, 1, -1, -1);
    add_downtime(content, now, now + 3, 3, true, 42, 24, 1, 1, now, -1);
    add_downtime(content, now, now + 3, 3, true, 42, 24, 1, 1, now, now + 3);

    // Check.
    check_content(t, content);

    // Success.
    retval = EXIT_SUCCESS;
  }
  catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "unknown exception" << std::endl;
  }

  ::unlink(persistent_cache_file.c_str());
  ::unlink(temporary_config_file.c_str());

  return (retval);
}
