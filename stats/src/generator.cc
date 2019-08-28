/*
** Copyright 2013-2015 Centreon
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

#include <chrono>
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/stats/builder.hh"
#include "com/centreon/broker/stats/config.hh"
#include "com/centreon/broker/stats/json_serializer.hh"
#include "com/centreon/broker/stats/generator.hh"
#include "com/centreon/broker/stats/metric.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

/**
 *  Default constructor.
 */
generator::generator() : _should_exit{false} {}

/**
 *  Destructor.
 */
generator::~generator() throw () {}

/**
 *  Request thread to exit ASAP.
 */
void generator::exit() {
  _should_exit = true;
}

/**
 *  Run generator thread.
 *
 *  @param[in] cfg         Stats configuration.
 *  @param[in] instance_id Instance ID.
 */
void generator::run(config const& cfg __attribute__((unused)), unsigned int instance_id) {
  // Set instance ID.
  _instance_id = instance_id;

  // Set exit flag.
  _should_exit = false;

  // Launch thread.
  _thread = std::thread(&generator::_run, this);
}

/**
 *  Thread entry point.
 */
void generator::_run() {
  try {
    time_t next_time(time(NULL) + 1);
    while (!_should_exit) {
      // Wait for appropriate time.
      time_t now(time(NULL));
      if (now < next_time) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue ;
      }
      next_time = now + 1;

      // Generate stats.
      logging::info(logging::medium)
        << "stats: time has come to generate statistics";
      builder b;
      b.build(json_serializer());

      // Send dumper events.
      {
        std::shared_ptr<dumper::dump> d(new dumper::dump);
        d->source_id = _instance_id;
        d->content = b.data().c_str();
        d->tag = "";
        multiplexing::publisher p;
        p.write(d);
      }
    }
  }
  catch (std::exception const& e) {
    logging::error(logging::high)
      << "stats: generator thread will exit due to the following error: "
      << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "stats: generator thread will exit due to an unknown error";
  }
  return ;
}

void generator::wait() {
  _thread.join();
}
