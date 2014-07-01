/*
** Copyright 2013-2014 Merethis
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

#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/stats/builder.hh"
#include "com/centreon/broker/stats/config.hh"
#include "com/centreon/broker/stats/generator.hh"
#include "com/centreon/broker/stats/metric.hh"

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
      b.build();

      // Send dumper events.
      {
        misc::shared_ptr<dumper::dump> d(new dumper::dump);
        d->content = b.data().c_str();
        d->instance_id = _instance_id;
        d->tag = _tag.c_str();
        multiplexing::publisher p;
        p.write(d);
      }

      // Send RRD events.
      for (std::list<io::properties>::const_iterator
             it1(b.root().children().begin()),
             end1(b.root().children().end());
           it1 != end1;
           ++it1) {
        for (std::map<std::string, io::property>::const_iterator
               it2(it1->begin()),
               end2(it1->end());
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
            ss->output = it2->second.get_output().c_str();
            ss->perf_data = it2->second.get_perfdata().c_str();
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
