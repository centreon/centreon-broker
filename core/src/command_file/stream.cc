/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/command_file/external_command.hh"
#include "com/centreon/broker/command_file/stream.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
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
  try :  _filename(filename),
         _fifo(filename.c_str()) {
  logging::debug(logging::medium)
    << "command_file: command file '" << filename << "' initialized";
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
 *  Read data from stream.
 *
 *  @param[out] d         Next available event.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  d.clear();
  time_t now(time(NULL));
  int timeout;
  if (now >= deadline)
    timeout = 0;
  else
    timeout = (deadline - now) * 1000000; // Microseconds timeout.
  std::string line = _fifo.read_line(timeout);
  if (!line.empty()) {
    line.erase(line.size() - 1); // Remove \n.
    misc::shared_ptr<external_command> exc(new external_command);
    exc->command = QString::fromStdString(line);
    d = exc;
    logging::info(logging::high)
      << "command_file: received external command: '"
      << exc->command << "'";
  }
  return (!line.empty());
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
  if (!d.isNull())
    throw (io::exceptions::shutdown(false, true)
           << "cannot write to command file '" << _filename << "'");
  return (1);
}
