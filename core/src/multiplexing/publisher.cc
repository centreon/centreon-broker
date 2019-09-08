/*
** Copyright 2011-2013,2017 Centreon
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

#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/multiplexing/engine.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
publisher::publisher() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
publisher::publisher(publisher const& other) : io::stream(other) {}

/**
 *  Destructor.
 */
publisher::~publisher() throw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& other) {
  io::stream::operator=(other);
  return (*this);
}

/**
 *  @brief Read data.
 *
 *  Reading is not available from publisher. Therefore this method will
 *  throw an exception.
 *
 *  @param[out] d         Unused.
 *  @param[in]  deadline  Timeout.
 */
bool publisher::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw(exceptions::shutdown() << "cannot read from publisher");
  return (true);
}

/**
 *  @brief Write data.
 *
 *  Send data to the multiplexing engine.
 *
 *  @param[in] d Multiplexed data.
 *
 *  @return Number of elements acknowledged (1).
 */
int publisher::write(std::shared_ptr<io::data> const& d) {
  engine::instance().publish(d);
  return (1);
}
