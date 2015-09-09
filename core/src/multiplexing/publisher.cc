/*
** Copyright 2011-2013 Centreon
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
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

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
publisher::publisher() : _process(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] p Object to copy.
 */
publisher::publisher(publisher const& p)
  : io::stream(p), _process(p._process) {}

/**
 *  Destructor.
 */
publisher::~publisher() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Object to copy.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& p) {
  io::stream::operator=(p);
  _process = p._process;
  return (*this);
}

/**
 *  Set whether or not to process inputs and outputs.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable publisher.
 */
void publisher::process(bool in, bool out) {
  (void)in;
  _process = out;
  return ;
}

/**
 *  @brief Read data.
 *
 *  Reading is not available from publisher. Therefore this method will
 *  throw an exception.
 *
 *  @param[out] d Unused.
 */
void publisher::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  // XXX : use io::exceptions::read_error
  throw (exceptions::msg()
           << "multiplexing: attempt to read from publisher");
  return ;
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
unsigned int publisher::write(misc::shared_ptr<io::data> const& d) {
  if (_process)
    engine::instance().publish(d);
  else
    throw (io::exceptions::shutdown(true, true) << "publisher "
             << this << " is shutdown");
  return (1);
}
