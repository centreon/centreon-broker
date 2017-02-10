/*
** Copyright 2017 Centreon
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/generator/dummy.hh"
#include "com/centreon/broker/generator/receiver.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::generator;

/**
 *  Constructor.
 */
receiver::receiver() {}

/**
 *  Destructor.
 */
receiver::~receiver() {}

/**
 *  Throw an exception.
 *
 *  @param[out] d
 *  @param[in]  deadline  Unused.
 *
 *  @return This method will throw.
 */
bool receiver::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from event receiver (generator)");
  return (true);
}

/**
 *  Receive a new event.
 *
 *  @param[in] d  New event.
 *
 *  @return 1.
 */
int receiver::write(misc::shared_ptr<io::data> const& d) {
  if (!d.isNull() && (d->type() == dummy::static_type())) {
    dummy const& e(d.ref_as<dummy>());

    // Find last number of the Broker instance.
    umap<unsigned int, unsigned int>::iterator
      it(_last_numbers.find(e.source_id));
    if (it == _last_numbers.end()) {
      _last_numbers[e.source_id] = 0;
      it = _last_numbers.find(e.source_id);
    }

    // Check current number.
    if (e.number == 1)
      it->second = e.number;
    else if (e.number != ++(it->second))
      throw (exceptions::msg()
             << "invalid sequence number for Centreon Broker instance "
             << e.source_id << ": got " << e.number << ", expected "
             << it->second - 1);
  }
  return (1);
}
