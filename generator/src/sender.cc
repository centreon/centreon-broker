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

#include "com/centreon/broker/generator/sender.hh"
#include <memory>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/generator/dummy.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::generator;

/**
 *  Constructor.
 */
sender::sender() : _number(0) {}

/**
 *  Destructor.
 */
sender::~sender() {}

/**
 *  Create a new dummy event.
 *
 *  @param[out] d         Set to a new dummy event.
 *  @param[in]  deadline  Unused.
 *
 *  @return True.
 */
bool sender::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d = std::make_shared<dummy>(dummy(++_number));
  return true;
}

/**
 *  Throw an exception.
 *
 *  @param[in] d  Unused.
 *
 *  @return This method will throw.
 */
int sender::write(std::shared_ptr<io::data> const& d) {
  (void)d;
  throw(exceptions::shutdown() << "cannot write to event sender (generator)");
  return 1;
}
