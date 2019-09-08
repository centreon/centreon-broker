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

#include "com/centreon/broker/generator/receiver.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/generator/dummy.hh"

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
bool receiver::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw(exceptions::shutdown()
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
int receiver::write(std::shared_ptr<io::data> const& d) {
  if (d && (d->type() == dummy::static_type())) {
    dummy const& e(*(static_cast<dummy*>(d.get())));

    // Find last number of the Broker instance.
    std::unordered_map<unsigned int, unsigned int>::iterator it(
        _last_numbers.find(e.source_id));
    if (it == _last_numbers.end()) {
      _last_numbers[e.source_id] = 0;
      it = _last_numbers.find(e.source_id);
    }

    // Check current number.
    ++(it->second);
    if (e.number > it->second)
      throw(exceptions::msg()
            << "invalid sequence number for Centreon Broker instance "
            << e.source_id << ": got " << e.number << ", expected "
            << it->second);
    it->second = e.number;
  }
  return (1);
}
