/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/io/stream.hh"

/**
 *  @class bench_stream bench_stream.hh "test/bench_stream.hh"
 *  @brief Benchmark stream.
 *
 *  Respect the io::stream interface for benchmark purpose.
 */
class           bench_stream : public com::centreon::broker::io::stream {
public:
                bench_stream();
                ~bench_stream();
  unsigned long get_write_events() const;
  unsigned long get_write_size() const;
  void          process(bool in = false, bool out = true);
  void          read(
                  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data>& d);
  void          reset_bench();
  unsigned int  write(
                  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> const& d);

private:
                bench_stream(bench_stream const& other);
  bench_stream& operator=(bench_stream const& other);

  unsigned long _write_events;
  unsigned long _write_size;
};
