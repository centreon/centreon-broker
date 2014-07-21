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

#ifndef TEST_BENCH_GENERATE_RRD_MOD_HH
#  define TEST_BENCH_GENERATE_RRD_MOD_HH

#  include <QCoreApplication>
#  include <QThread>
#  include <QTimer>
#  include "com/centreon/broker/multiplexing/engine.hh"
#  include "com/centreon/broker/multiplexing/publisher.hh"
#  include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;

/**
 *  Benchmark class.
 */
class  benchmark : public QThread {
  Q_OBJECT

public:
       benchmark(
           unsigned int services,
           unsigned int requests_per_service)
    : _services(services),
      _requests_per_service(requests_per_service) {}
       ~benchmark() {}
  void run() {
    // Wait for multiplexing engine to come online.
    while (multiplexing::engine::instance().stopped())
      yieldCurrentThread();

    // Publisher.
    multiplexing::publisher publicheur;

    // Send requests.
    time_t now(time(NULL));
    for (unsigned int i(0); i < _requests_per_service; ++i) {
      now += 300;
      for (unsigned int j(0); j < _services; ++j) {
        misc::shared_ptr<storage::metric>
          m(new storage::metric);
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

    return ;
  }

private:
  unsigned int _services;
  unsigned int _requests_per_service;
};

#endif // !TEST_BENCH_GENERATE_RRD_MOD_HH
