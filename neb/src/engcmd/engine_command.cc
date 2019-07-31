/*
** Copyright 2015,2017 Centreon
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

#include <cstdio>
#include <QLibrary>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/extcmd/command_listener.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/command_result.hh"
#include "com/centreon/broker/neb/engcmd/engine_command.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

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
       std::shared_ptr<io::data>& d,
       time_t deadline) {
  (void)deadline;
  d.reset();
  throw (exceptions::shutdown() << "cannot read from engine command");
  return (true);
}

/**
 *  Write to stream.
 *
 *  @param[in] d  The event to write.
 *
 *  @return  See io::stream::read().
 */
int engine_command::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, "engine command"))
    return 1;

  if (d->type() == command_request::static_type()) {
    command_request const& request = *std::static_pointer_cast<command_request const>(d);

    if (request.destination_id == config::applier::state::instance().poller_id()
        && request.endp == _name) {
      _execute_command(request.endp);
      std::shared_ptr<command_result> result{new command_result};
      result->code = 1;
      result->uuid = request.uuid;
      result->msg = "\"Command successfully sent to engine\"";
      multiplexing::publisher pblsh;
      pblsh.write(result);
    }
  }

  return 1;
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
