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

#include "com/centreon/broker/command_file/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/namespace.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::command_file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stream::stream(std::string const& filename)
  try : _filename(filename),
        _fifo(filename.c_str()) {
}
catch (std::exception const& e) {
  throw (exceptions::msg()
         << "command_file: error while initializing command file: "
         << e.what());
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Set which data should be processed.
 *
 *  @param[in] in  Set to true to process input events.
 *  @param[in] out Set to true to process output events.
 */
void stream::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
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
  d.clear();

  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "command file stream is shutdown");

  std::string line = _fifo.read_line();
  if (!line.empty())
    d = _parse_command_line(line);
}

/**
 *  Get statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  (void)tree;
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
  throw (exceptions::msg()
         << "command_file: attempt to write to a command file");
  return (1);
}

/**
 *  Parse a command line and generate an event.
 *
 *  @param[in] line  The line to be parsed.
 *
 *  @return          The event.
 */
misc::shared_ptr<io::data>
  stream::_parse_command_line(std::string const& line) {
  return (misc::shared_ptr<io::data>());
}
