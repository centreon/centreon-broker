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

#include "test/bench_stream.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

/**
 *  Default constructor.
 */
bench_stream::bench_stream() : _write_events(0), _write_size(0) {}

/**
 *  Destructor.
 */
bench_stream::~bench_stream() {}

/**
 *  Get the number of events written to this stream.
 *
 *  @return Number of events written to this stream.
 */
unsigned long bench_stream::get_write_events() const {
  return (_write_events);
}

/**
 *  Get the size of events written to this stream.
 *
 *  @return Total size of events written to this stream.
 */
unsigned long bench_stream::get_write_size() const {
  return (_write_size);
}

/**
 *  @brief Read event.
 *
 *  In this class it does nothing.
 *
 *  @param[out] d         Unused.
 *  @param[in]  deadline  Timeout.
 *
 *  @return True.
 */
bool bench_stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  return (true);
}

/**
 *  Reset benchmark counters.
 */
void bench_stream::reset_bench() {
  _write_events = 0;
  _write_size = 0;
  return;
}

/**
 *  Write event.
 *
 *  @param[in] d  Event.
 *
 *  @return Number of events processed (1).
 */
int bench_stream::write(std::shared_ptr<io::data> const& d) {
  if (!d.isNull() && (d->type() == io::raw::static_type())) {
    std::shared_ptr<io::raw> r(d.staticCast<io::raw>());
    ++_write_events;
    _write_size += r->size();
  }
  return (1);
}
