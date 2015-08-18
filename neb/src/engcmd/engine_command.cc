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

#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/neb/engcmd/engine_command.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker::neb::engcmd;
using namespace com::centreon::broker::extcmd;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the stream.
 */
engine_command::engine_command(std::string const& name)
  : _name(name) {}

/**
 *  Destructor.
 */
engine_command::~engine_command() {}

/**
 *  Read from stream.
 *
 *  @param[in] d         Unusued.
 *  @param[in] deadline  Unusued.
 *
 *  @return              True.
 */
bool engine_command::read(
       misc::shared_ptr<io::data>& d,
       time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from engine command");
  return (true);
}

/**
 *  Write to stream.
 *
 *  @param[in] d  The event to write.
 *
 *  @return  See io::stream::read().
 */
unsigned int engine_command::write(misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return (1);

  if (d->type() == command_request::static_type()) {
    command_request const& request = d.ref_as<command_request const>();

    if (request.destination_id == config::applier::state::instance().poller_id()
        && request.endp == QString::fromStdString(_name)) {
      _execute_command(request.endp.toStdString());
    }
  }

  return (1);
}

/**
 *  Execute a command.
 *
 *  @param[in] cmd  The command.
 */
void engine_command::_execute_command(std::string const cmd) {

}
