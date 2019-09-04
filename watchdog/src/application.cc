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

#include "com/centreon/broker/watchdog/application.hh"
#include <sys/socket.h>
#include <unistd.h>
#include <set>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/watchdog/configuration_parser.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

int application::sighup_fd[2];
int application::sigterm_fd[2];

/**
 *  Constructor.
 *
 *  @param[in] config_file  The config file.
 */
application::application(std::string const& config_file)
    : _config_path(config_file) {
  // Init the socketpairs used for signal handling.
  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighup_fd) != 0 ||
      ::socketpair(AF_UNIX, SOCK_STREAM, 0, sigterm_fd) != 0)
    throw(exceptions::msg() << "watchdog: could not init the socket pairs");
  _sighup.reset(new QSocketNotifier(sighup_fd[1], QSocketNotifier::Read, this));
  connect(_sighup.get(), SIGNAL(activated(int)), this, SLOT(handle_sighup()));
  _sigterm.reset(
      new QSocketNotifier(sigterm_fd[1], QSocketNotifier::Read, this));
  connect(_sigterm.get(), SIGNAL(activated(int)), this, SLOT(handle_sigterm()));

  _init();
}

/**
 *  Destructor.
 */
application::~application() {
  for (std::map<std::string, instance*>::iterator it(_instances.begin()),
       end(_instances.end());
       it != end; ++it)
    delete it->second;
  logging::manager::unload();
}

/**
 *  Handle sighup.
 */
void application::handle_sighup() {
  _sighup->setEnabled(false);
  char tmp;
  ::read(sighup_fd[1], &tmp, sizeof(tmp));

  configuration config;
  try {
    configuration_parser parser;
    config = parser.parse(_config_path);
  } catch (std::exception const& e) {
    logging::error(logging::medium)
        << "watchdog: could not parse the new configuration: " << e.what();
    _sighup->setEnabled(true);
    return;
  }

  _apply_new_configuration(config);
  _sighup->setEnabled(true);
}

/**
 *  Handle sigterm.
 */
void application::handle_sigterm() {
  _sigterm->setEnabled(false);
  char tmp;
  ::read(sigterm_fd[1], &tmp, sizeof(tmp));
  _quit();
  _sigterm->setEnabled(true);
  return;
}

/**
 *  Initialize the application.
 */
void application::_init() {
  // Parse the configuration.
  configuration_parser parser;
  configuration config = parser.parse(_config_path);

  // Load the log manager.
  logging::manager::load();

  // Apply the configuration.
  _apply_new_configuration(config);
}

/**
 *  Apply a new configuration.
 *
 *  @param[in] config  The new configuration.
 */
void application::_apply_new_configuration(configuration const& config) {
  // Create the log file backend if needed.
  if (_config.get_log_filename() != config.get_log_filename()) {
    _log.reset(new logging::file(config.get_log_filename()));
    logging::manager::instance().log_on(*_log);
  }

  std::set<std::string> to_update;
  std::set<std::string> to_delete;
  std::set<std::string> to_create;

  // Old configs that aren't present in the new should be deleted.
  // Old configs that are present in the new should be updated
  // or deleted/recreated.
  for (configuration::instance_map::const_iterator
           it(_config.get_instances_configuration().begin()),
       end(_config.get_instances_configuration().end());
       it != end; ++it) {
    instance_configuration new_config(
        config.get_instance_configuration(it->first));
    if (new_config.is_empty())
      to_delete.insert(it->first);
    else if (new_config != it->second) {
      to_delete.insert(it->first);
      to_create.insert(it->first);
    } else
      to_update.insert(it->first);
  }

  // New configs that aren't present in the old should be created.
  for (configuration::instance_map::const_iterator
           it(config.get_instances_configuration().begin()),
       end(config.get_instances_configuration().end());
       it != end; ++it)
    if (!_config.instance_exists(it->first))
      to_create.insert(it->first);

  // Delete old processes.
  for (std::set<std::string>::const_iterator it(to_delete.begin()),
       end(to_delete.end());
       it != end; ++it) {
    std::map<std::string, instance*>::iterator found(_instances.find(*it));
    if (found != _instances.end()) {
      delete found->second;
      _instances.erase(found);
    }
  }

  // Update processes.
  for (std::set<std::string>::const_iterator it(to_update.begin()),
       end(to_update.end());
       it != end; ++it) {
    std::map<std::string, instance*>::iterator found(_instances.find(*it));
    if (found != _instances.end()) {
      found->second->merge_configuration(
          config.get_instance_configuration(*it));
      found->second->update_instance();
    }
  }

  // Start new processes.
  for (std::set<std::string>::const_iterator it(to_create.begin()),
       end(to_create.end());
       it != end; ++it) {
    std::unique_ptr<instance> ins(
        new instance(config.get_instance_configuration(*it), *this));
    _instances.insert(std::make_pair(*it, ins.release()));
  }

  // Save the new configuration.
  _config = config;
}

/**
 *  Quit the application and subinstances.
 */
void application::_quit() {
  logging::info(logging::medium) << "watchdog: initiating shutdown sequence";
  for (std::map<std::string, instance*>::iterator it(_instances.begin()),
       end(_instances.end());
       it != end; ++it)
    it->second->stop_instance();
  logging::info(logging::medium)
      << "watchdog: shutdown sequence completed, exiting watchdog";
  exit();
  return;
}
