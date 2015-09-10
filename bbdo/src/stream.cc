/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/bbdo/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/namespace.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 *
 *  @param[in] is_in  Is input ?
 *  @param[in] is_out Is output ?
 */
stream::stream(bool is_in, bool is_out)
  : _input_read(is_in), _output_write(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
stream::stream(stream const& right)
  : io::stream(right),
    input(right),
    output(right),
    _input_read(right._input_read),
    _output_write(right._output_write) {}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& right) {
  if (this != &right) {
    input::operator=(right);
    output::operator=(right);
    _input_read = right._input_read;
    _output_write = right._output_write;
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
 *  @param[out] d Next available event.
 *
 *  @see input::read()
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  if (_input_read)
    input::read(d);
  else
    output::read(d);
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
  if (_output_write)
    output::write(d);
  else
    input::write(d);
  return (1);
}
