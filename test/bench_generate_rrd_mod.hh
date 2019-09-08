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

#ifndef TEST_BENCH_GENERATE_RRD_MOD_HH
#define TEST_BENCH_GENERATE_RRD_MOD_HH

#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;

/**
 *  Benchmark class.
 */
class benchmark : public QThread {
  Q_OBJECT

 public:
  benchmark(unsigned int services, unsigned int requests_per_service)
      : _services(services), _requests_per_service(requests_per_service) {}
  ~benchmark() {}
  void run() {
    // Wait for multiplexing engine to come online.
    // XXX
    // while (multiplexing::engine::instance().stopped())
    //   yieldCurrentThread();

    // Publisher.
    multiplexing::publisher publicheur;

    // Send requests.
    time_t now(time(NULL));
    for (unsigned int i(0); i < _requests_per_service; ++i) {
      now += 300;
      for (unsigned int j(0); j < _services; ++j) {
        std::shared_ptr<storage::metric> m(new storage::metric);
        m->ctime = now;
        m->interval = 300;
        m->is_for_rebuild = false;
        m->metric_id = j + 1;
        m->name = "value";
        m->rrd_len = 180 * 24 * 3600;
        m->value = i;
        publicheur.write(m);
      }
    }

    // Quit process.
    QTimer::singleShot(0, QCoreApplication::instance(), SLOT(quit()));

    return;
  }

 private:
  unsigned int _services;
  unsigned int _requests_per_service;
};

#endif  // !TEST_BENCH_GENERATE_RRD_MOD_HH
