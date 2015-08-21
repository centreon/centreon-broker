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

#include <cstdio>
#include <QLibrary>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/neb/engcmd/engine_command.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"

using namespace com::centreon::broker::neb::engcmd;
using namespace com::centreon::broker::extcmd;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the stream.
 */
engine_command::engine_command(
                  std::string const& name,
                  std::string const& command_module_path)
  : _name(name),
    _command_module_path(command_module_path),
    _process_external_command(NULL) {
  _load_command_engine_module();
}

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
      misc::shared_ptr<command_result> result(new command_result);
      result->code = 1;
      result->id = request.id;
      result->msg = "Command successfully sent to engine";
      multiplexing::publisher pblsh;
      pblsh.write(result);
    }
  }

  return (1);
}

/**
 *  Load the command engine module.
 */
void engine_command::_load_command_engine_module() {
  QLibrary lib(QString::fromStdString(_command_module_path));

  if (!lib.load())
    throw (exceptions::msg()
           << "engcmd: couldn't load '"
           << _command_module_path << "': " << lib.errorString());

  union {
    void* ptr;
    int (*f)(char const*);
  } ptr_to_f;
  ptr_to_f.ptr = lib.resolve("process_external_command");
  if (ptr_to_f.ptr == NULL)
    throw (exceptions::msg()
           << "engcmd: couldn't resolve 'process_external_command': "
           << lib.errorString());
  _process_external_command = ptr_to_f.f;

}

/**
 *  Execute a command.
 *
 *  @param[in] cmd  The command.
 */
void engine_command::_execute_command(std::string const cmd) {
  char buff[32];
  int ret = ::snprintf(buff, 32, "[%li]", ::time(NULL));
  if (ret < 0 || ret >= 32)
    return ;

  std::string prepared_cmd;
  prepared_cmd.append(buff).append(" ").append(cmd);

  (*_process_external_command)(prepared_cmd.c_str());
}
