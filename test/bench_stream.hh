/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/io/stream.hh"

/**
 *  @class bench_stream bench_stream.hh "test/bench_stream.hh"
 *  @brief Benchmark stream.
 *
 *  Respect the io::stream interface for benchmark purpose.
 */
class bench_stream : public com::centreon::broker::io::stream {
 public:
  bench_stream();
  ~bench_stream();
  unsigned long get_write_events() const;
  unsigned long get_write_size() const;
  bool read(std::shared_ptr<com::centreon::broker::io::data>& d,
            time_t deadline);
  void reset_bench();
  int write(std::shared_ptr<com::centreon::broker::io::data> const& d);

 private:
  bench_stream(bench_stream const& other);
  bench_stream& operator=(bench_stream const& other);

  unsigned long _write_events;
  unsigned long _write_size;
};
