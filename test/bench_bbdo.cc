/*
** Copyright 2014 Centreon
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
#include <ctime>
#include <iomanip>
#include <iostream>
#include <QCoreApplication>
#include <sys/time.h>
#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/ndo/stream.hh"
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
 *  @param[in] ndos   NDO stream.
 *  @param[in] ndob   NDO benchmark.
 *  @param[in] bbdos  BBDO stream.
 *  @param[in] bbdob  BBDO benchmark.
 */
static void benchmark_streams(
              ndo::stream& ndos,
              bench_stream& ndob,
              bbdo::stream& bbdos,
              bench_stream& bbdob) {
  // Benchmark NDO stream.
  std::cout << "  NDO\n";
  send_events(&ndos);
  std::cout << "  - events  " << ndob.get_write_events() << "\n"
            << "  - size    " << ndob.get_write_size() << "\n";

  // Benchmark BBDO stream.
  std::cout << "  BBDO\n";
  send_events(&bbdos);
  std::cout << "  - events  " << bbdob.get_write_events() << "\n"
            << "  - size    " << bbdob.get_write_size() << "\n";

  // Diff.
  std::cout << std::setprecision(2) << "  DIFF (NDO is 100%)\n"
    << "  - events  +" << std::fixed
    << (bbdob.get_write_events() * 100.0 / ndob.get_write_events() - 100.0)
    << "%\n  - size    " << std::fixed
    << ((bbdob.get_write_size() * 100.0 / bbdob.get_write_events())
        / (ndob.get_write_size() / ndob.get_write_events()) - 100.0)
    << "%\n";
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
    ndo::stream ndos;
    misc::shared_ptr<bench_stream> ndob(new bench_stream);
    ndos.write_to(ndob);
    bbdo::stream bbdos(true, true);
    misc::shared_ptr<bench_stream> bbdob(new bench_stream);
    bbdos.write_to(bbdob);
    benchmark_streams(ndos, *ndob, bbdos, *bbdob);
  }

  // #2 Default compression.
  std::cout << "\nBench #2 (default compression)\n";
  {
    ndo::stream ndos;
    misc::shared_ptr<compression::stream> ndoc(new compression::stream);
    misc::shared_ptr<bench_stream> ndob(new bench_stream);
    ndos.write_to(ndoc);
    ndoc->write_to(ndob);
    bbdo::stream bbdos(true, true);
    misc::shared_ptr<compression::stream> bbdoc(new compression::stream);
    misc::shared_ptr<bench_stream> bbdob(new bench_stream);
    bbdos.write_to(bbdoc);
    bbdoc->write_to(bbdob);
    benchmark_streams(ndos, *ndob, bbdos, *bbdob);
  }

  // #3 Optimized compression.
  std::cout << "\nBench #3 (optimized compression)\n";
  {
    ndo::stream ndos;
    misc::shared_ptr<compression::stream>
      ndoc(new compression::stream(9, 1000000));
    misc::shared_ptr<bench_stream> ndob(new bench_stream);
    ndos.write_to(ndoc);
    ndoc->write_to(ndob);
    bbdo::stream bbdos(true, true);
    misc::shared_ptr<compression::stream>
      bbdoc(new compression::stream(9, 1000000));
    misc::shared_ptr<bench_stream> bbdob(new bench_stream);
    bbdos.write_to(bbdoc);
    bbdoc->write_to(bbdob);
    benchmark_streams(ndos, *ndob, bbdos, *bbdob);
  }

  // Cleanup.
  config::applier::deinit();

  return (EXIT_SUCCESS);
}
