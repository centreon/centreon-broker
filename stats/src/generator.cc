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

#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/stats/builder.hh"
#include "com/centreon/broker/stats/config.hh"
#include "com/centreon/broker/stats/generator.hh"
#include "com/centreon/broker/stats/metric.hh"
#include "com/centreon/broker/stats/plain_text_serializer.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::stats;

/**
 *  Default constructor.
 */
generator::generator() {}

/**
 *  Destructor.
 */
generator::~generator() throw () {}

/**
 *  Request thread to exit ASAP.
 */
void generator::exit() {
  _should_exit = true;
  return ;
}

/**
 *  Run generator thread.
 *
 *  @param[in] cfg         Stats configuration.
 *  @param[in] instance_id Instance ID.
 */
void generator::run(config const& cfg, unsigned int instance_id) {
  // Set interval.
  _interval = cfg.get_interval();

  // Set instance ID.
  _instance_id = instance_id;

  // Set tag.
  _tag = cfg.get_dumper_tag();

  // Get IDs.
  for (std::list<metric>::const_iterator
         it(cfg.metrics().begin()), end(cfg.metrics().end());
       it != end;
       ++it)
    _plugins[it->get_name()]
      = std::make_pair(it->get_host_id(), it->get_service_id());

  // Set exit flag.
  _should_exit = false;

  // Launch thread.
  start();

  return ;
}

/**
 *  Thread entry point.
 */
void generator::run() {
  try {
    time_t next_time(time(NULL) + _interval);
    while (!_should_exit) {
      // Wait for appropriate time.
      time_t now(time(NULL));
      if (now < next_time) {
        sleep(1);
        continue ;
      }
      next_time = now + _interval;

      // Generate stats.
      logging::info(logging::medium)
        << "stats: time has come to generate statistics (tag '"
        << _tag << "')";
      builder b;
      b.build(plain_text_serializer());

      // Send dumper events.
      {
        misc::shared_ptr<dumper::dump> d(new dumper::dump);
        d->source_id = _instance_id;
        d->content = b.data().c_str();
        d->tag = _tag.c_str();
        multiplexing::publisher p;
        p.write(d);
      }

      // Send RRD events.
      for (io::properties::children_list::const_iterator
             it1(b.root().children().begin()),
             end1(b.root().children().end());
           it1 != end1;
           ++it1) {
        for (std::map<std::string, io::property>::const_iterator
               it2(it1->second.begin()),
               end2(it1->second.end());
             it2 != end2;
             ++it2) {
          std::map<
                std::string,
                std::pair<unsigned int, unsigned int> >::const_iterator
            it3(_plugins.find(it2->first));
          if ((it3 != _plugins.end()) && it2->second.is_graphable()) {
            logging::debug(logging::low)
              << "stats: generating update for graphable service '"
              << it3->first << "' (host " << it3->second.first
              << ", service " << it3->second.second << ")";
            misc::shared_ptr<neb::service_status>
              ss(new neb::service_status);
            ss->host_id = it3->second.first;
            ss->service_id = it3->second.second;
            // XXX ss->host_name = instance_name;
            ss->service_description = it3->first.c_str();
            ss->output = "";
            ss->perf_data =
                  QString::fromStdString(
                             it2->second.get_name()
                             + "="
                             + it2->second.get_value());
            multiplexing::publisher p;
            p.write(ss);
          }
        }
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
