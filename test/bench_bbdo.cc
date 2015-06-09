/*
** Copyright 2014-2015 Merethis
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

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <QCoreApplication>
#include <sys/time.h>
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/neb/events.hh"
#include "test/bench_stream.hh"

#define TEST_TIME 30

using namespace com::centreon::broker;

/**
 *  Mesure time performance.
 */
static void send_events(io::stream* s) {
  // Compute time limit.
  timespec limit;
  clock_gettime(CLOCK_REALTIME, &limit);
  limit.tv_sec += TEST_TIME;
  timespec ts;

  // Loop.
  do {
    // Send event.
    misc::shared_ptr<neb::service_status> ss(new neb::service_status);
    s->write(ss);

    // Compute current time.
    clock_gettime(CLOCK_REALTIME, &ts);
  } while ((ts.tv_sec < limit.tv_sec)
           || ((ts.tv_sec == limit.tv_sec)
               && (ts.tv_nsec < limit.tv_nsec)));

  // Forced commit (might be needed by the compression).
  s->write(misc::shared_ptr<io::data>());

  return ;
}

/**
 *  Benchmark two streams.
 *
 *  @param[in] bbdos  BBDO stream.
 *  @param[in] bbdob  BBDO benchmark.
 */
static void benchmark_stream(
              bbdo::stream& bbdos,
              bench_stream& bbdob) {
  // Benchmark BBDO stream.
  std::cout << "  BBDO\n";
  send_events(&bbdos);
  std::cout << "  - events  " << bbdob.get_write_events() << "\n"
            << "  - size    " << bbdob.get_write_size() << "\n";
  return ;
}

/**
 *  Compare BBDO performance to NDO.
 *
 *  @param[in] argc  Argument count.
 *  @param[in] argv  Argument values.
 *
 *  @return EXIT_SUCCESS.
 */
int main(int argc, char* argv[]) {
  // Initialization.
  QCoreApplication app(argc, argv);
  config::applier::init();

  // #1 Default streams.
  std::cout << "Bench #1 (default streams)\n";
  {
    bbdo::stream bbdos;
    misc::shared_ptr<bench_stream> bbdob(new bench_stream);
    bbdos.set_substream(bbdob);
    benchmark_stream(bbdos, *bbdob);
  }

  // #2 Default compression.
  std::cout << "\nBench #2 (default compression)\n";
  {
    bbdo::stream bbdos;
    misc::shared_ptr<compression::stream> bbdoc(new compression::stream);
    misc::shared_ptr<bench_stream> bbdob(new bench_stream);
    bbdos.set_substream(bbdoc);
    bbdoc->set_substream(bbdob);
    benchmark_stream(bbdos, *bbdob);
  }

  // #3 Optimized compression.
  std::cout << "\nBench #3 (optimized compression)\n";
  {
    bbdo::stream bbdos;
    misc::shared_ptr<compression::stream>
      bbdoc(new compression::stream(9, 1000000));
    misc::shared_ptr<bench_stream> bbdob(new bench_stream);
    bbdos.set_substream(bbdoc);
    bbdoc->set_substream(bbdob);
    benchmark_stream(bbdos, *bbdob);
  }

  // Cleanup.
  config::applier::deinit();

  return (EXIT_SUCCESS);
}
