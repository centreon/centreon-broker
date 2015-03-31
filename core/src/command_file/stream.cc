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

#include <cstdio>
#include <vector>
#include "com/centreon/broker/command_file/external_command.hh"
#include "com/centreon/broker/command_file/stream.hh"
#include "com/centreon/broker/misc/string.hh"
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
  if (!line.empty()) {
    line.erase(line.size() - 1);
    misc::shared_ptr<external_command> exc(new external_command);
    exc->command = QString::fromStdString(line);
    d = exc;
  }
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
/*misc::shared_ptr<io::data>
  stream::_parse_command_line(std::string const& line) {

  std::string command;
  std::string args;
  command.resize(line.size());
  args.resize(line.size());

  // Parse timestamp.
  unsigned long timestamp;
  if (::sscanf(
        line.c_str(),
        "[%lu] %[^ ;];%s",
        &timestamp,
        &command[0],
        &args[0]) != 3)
    throw (exceptions::msg()
           << "couldn't parse the line");

  if (command == "ACKNOWLEDGE_HOST_PROBLEM")
    return (_parse_ack(ack_host, timestamp, args));
  else if (command == "ACKNOWLEDGE_SERVICE_PROBLEM")
    return (_parse_ack(ack_service, timestamp, args));

  return (misc::shared_ptr<io::data>());
}

/**
 *  Parse an acknowledgment.
 *
 *  @param[in] is_host  Is this an host acknowledgement.
 *  @param[in] t        The timestamp.
 *  @param[in] args     The args to parse.
 *
 *  @return             An acknowledgement event.
 */
/*misc::shared_ptr<io::data> stream::_parse_ack(
                             ack_type is_host,
                             timestamp t,
                             std::string const& args) {
  unsigned int host_id = 0;
  unsigned int service_id = 0;
  int sticky = 0;
  int notify = 0;
  int persistent_comment = 0;
  std::string author;
  std::string comment;
  author.resize(args.size());
  comment.resize(args.size());
  bool ret = false;
  if (is_host == ack_host)
    ret = (::sscanf(
             args.c_str(),
             "%u;%i;%i;%i;%[^;];%[^;]",
             &host_id,
             &sticky,
             &notify,
             &persistent_comment,
             &author[0],
             &comment[0]) == 6);
  else
    ret = (::sscanf(
             args.c_str(),
             "%u;%u;%i;%i",
             &host_id,
             &service_id,
             &sticky,
             &notify,
             &persistent_comment,
             &author[0],
             &comment[0]) == 7);
  if (!ret)
    throw (exceptions::msg()
           << "couldn't parse the arguments for the acknowledgement");

  misc::shared_ptr<neb::acknowledgement> ack(new neb::acknowledgement);
  ack->acknowledgement_type = is_host;
  ack->comment = comment;
  ack->author = author;
  ack->entry_time = t;
  ack->host_id = host_id;
  ack->service_id = service_id;
  ack->is_sticky = (sticky == 2);
  ack->persistent_comment = (persistent_comment == 1);
  ack->notify_contacts = (notify == 1);

  return (ack);
}

/**
 *  Parse a downtime.
 *
 *  @param[in] args     The args to parse.
 *  @param[in] t        The timestamp.
 *
 *  @return             A downtime event.
 */
/*misc::shared_ptr<io::data> stream::_parse_downtime(
                             timestamp t,
                             std::string const& args) {

}*/
