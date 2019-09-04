/*
 * Copyright 2015,2017 Centreon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 */

#include <sys/select.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <set>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/logging/manager.hh"
#include "com/centreon/broker/watchdog/application.hh"
#include "com/centreon/broker/watchdog/configuration.hh"
#include "com/centreon/broker/watchdog/configuration_parser.hh"
#include "com/centreon/broker/watchdog/instance.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

static char const* help_msg = "USAGE: cbwd configuration_file";
static bool should_exit{false};
static std::unique_ptr<logging::file> log;
static configuration config;
static std::unordered_map<std::string, instance*> instances;

/**
 *  Print the help.
 */
static void print_help() {
  std::cout << help_msg << std::endl;
}

/**
 *  @brief Signal handler
 *
 *  There is not many things we can do in signal handlers.
 *  We convert the signal into something usable by using the
 *  writing to a self-pipe pattern.
 *
 *  @param sig  The signal.
 */
static void signal_handler(int sig) {
  // Sigterm : should exit now...
  if (sig == SIGTERM || sig == SIGINT) {
    should_exit = true;
  // Sighup : should reload...
  } else if (sig == SIGHUP) {
    std::cout << "Reload\n";
  }
}

/**
 *  Set the signals handlers.
 */
static void set_signal_handlers() {
  struct sigaction sig;
  sig.sa_handler = signal_handler;
  ::sigemptyset(&sig.sa_mask);
  ::sigfillset(&sig.sa_mask);
  sig.sa_flags = 0;
  if (::sigaction(SIGTERM, &sig, NULL) < 0 ||
      ::sigaction(SIGINT, &sig, NULL) < 0 ||
      ::sigaction(SIGHUP, &sig, NULL) < 0)
    throw com::centreon::broker::exceptions::msg()
          << "can't set the signal handlers";
}

void apply_new_configuration(configuration const& cfg) {
  if (config.get_log_filename() != cfg.get_log_filename()) {
    log.reset(new logging::file(cfg.get_log_filename()));
    logging::manager::instance().log_on(*log);
  }

  std::set<std::string> to_update;
  std::set<std::string> to_delete;
  std::set<std::string> to_create;

  // Old configs that aren't present in the new should be deleted.
  // Old configs that are present in the new should be updated
  // or deleted/recreated.
  for (configuration::instance_map::const_iterator
           it(config.get_instances_configuration().begin()),
       end(config.get_instances_configuration().end());
       it != end; ++it) {
    instance_configuration new_config(
        cfg.get_instance_configuration(it->first));
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
           it(cfg.get_instances_configuration().begin()),
       end(cfg.get_instances_configuration().end());
       it != end; ++it)
    if (!config.instance_exists(it->first))
      to_create.insert(it->first);

  // Delete old processes.
  for (std::set<std::string>::const_iterator it(to_delete.begin()),
       end(to_delete.end());
       it != end; ++it) {
    std::unordered_map<std::string, instance*>::iterator found(instances.find(*it));
    if (found != instances.end()) {
      delete found->second;
      instances.erase(found);
    }
  }

  // Update processes.
  for (std::set<std::string>::const_iterator it(to_update.begin()),
       end(to_update.end());
       it != end; ++it) {
    std::unordered_map<std::string, instance*>::iterator found(instances.find(*it));
    if (found != instances.end()) {
      found->second->merge_configuration(
          cfg.get_instance_configuration(*it));
      found->second->update_instance();
    }
  }

  // Start new processes.
  for (std::set<std::string>::const_iterator it(to_create.begin()),
       end(to_create.end());
       it != end; ++it) {
    std::unique_ptr<instance> ins(
        new instance(cfg.get_instance_configuration(*it)));
    instances.insert({*it, ins.release()});
  }

  // Save the new configuration.
  config = cfg;
}

/**
 *  Centreon Watchdog entry point.
 *
 *  @param[in] argc  Parameter count.
 *  @param[in] argv  Parameter values.
 *
 *  @return  0 on success.
 */
int main(int argc, char** argv) {
  // Check arguments.
  if (argc != 2 || ::strcmp(argv[1], "-h") == 0) {
    print_help();
    return 0;
  }

  char const* config_filename = argv[1];

  // Load the log manager.
  logging::manager::load();

  configuration config;
  try {
    configuration_parser parser;
    config = parser.parse(config_filename);
  }
  catch (std::exception const& e) {
    std::cout << "ERROR: could not parse the configuration file '"
              << config_filename << "': " << e.what() << '\n';
    logging::error(logging::medium)
        << "watchdog: could not parse the new configuration: " << e.what();
    return 2;
  }

  int retval{0};
  signal(SIGCHLD, SIG_IGN);

  // Create the main event loop.
  try {
    set_signal_handlers();
    apply_new_configuration(config);
    while (!should_exit) {
      for (std::unordered_map<std::string, instance*>::iterator
             it{instances.begin()}, end{instances.end()};
             it != end; ++it) {
        fd_set fds;
        FD_ZERO(&fds);
        int fd{it->second->get_fd()};
        FD_SET(fd, &fds);
        struct timeval timeout{ .tv_sec = 0, .tv_usec = 1000 };
        if (select(fd + 1, &fds, nullptr, nullptr, &timeout) == 1) { // data available
          logging::error(logging::medium)
            << "watchdog: cbd '" << it->first << "' is lost. Attempt to restart it.";
          it->second->restart();
        }
      }

      int timeout = 5;
      while (timeout > 0) {
        timeout--;
        sleep(1);
        if (should_exit)
          break;
      }
    }
  } catch (std::exception const& e) {
    std::cerr << "watchdog: " << e.what() << std::endl;
    retval = 1;
  }

  for (std::unordered_map<std::string, instance*>::iterator
      it{instances.begin()}, end{instances.end()};
      it != end; ++it) {
    it->second->stop();
  }
  // Load the log manager.
  logging::manager::unload();

  return retval;
}
