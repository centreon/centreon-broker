/*
** Copyright 2013 Merethis
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
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(std::string& buffer) const {
  output::statistics(buffer);
  return ;
}

/**
 *  Write data to stream.
 *
 *  @param[in] d Data to send.
 */
void stream::write(misc::shared_ptr<io::data> const& d) {
  if (_output_write)
    output::write(d);
  else
    input::write(d);
  return ;
}
