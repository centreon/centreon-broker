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

#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/ndo/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stream::stream() {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s), input(s), output(s) {}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  if (this != &s) {
    input::operator=(s);
    output::operator=(s);
  }
  return (*this);
}

/**
 *  Set which data should be processed.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void stream::process(bool in, bool out) {
  input::process(in, false);
  output::process(false, out);
  return ;
}

/**
 *  Read data from stream.
 *
 *  @param[out] d Next available data.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  input::read(d);
  return ;
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  output::statistics(tree);
  return ;
}

/**
 *  Write data to stream.
 *
 *  @param[in] d Data to send.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& d) {
  return (output::write(d));
}
