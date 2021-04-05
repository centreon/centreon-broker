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

#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <set>
#include <unordered_map>

#include "com/centreon/broker/watchdog/configuration.hh"
#include "com/centreon/broker/watchdog/configuration_parser.hh"
#include "com/centreon/broker/watchdog/instance.hh"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

static char const* config_filename = nullptr;
static bool should_exit{false};
std::unique_ptr<spdlog::logger> logger;
static configuration config;
static std::unordered_map<std::string, instance*> instances;
static bool sighup{false};

/**
 *  Print the help.
 */
static void print_help() {
  std::cout << "USAGE: cbwd configuration_file" << std::endl;
}

/**
 *  This function applies a new configuration in replacement of the current one.
 *
 * @param cfg The new configuration to set.
 */
static void apply_new_configuration(configuration const& cfg) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::warn);
  console_sink->set_pattern("[cbwd] [%^%l%$] %v");

  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      cfg.get_log_filename(), false);
  file_sink->set_level(spdlog::level::trace);

  if (config.get_log_filename() != cfg.get_log_filename())
    logger.reset(new spdlog::logger("cbwd", {console_sink, file_sink}));
  else
    logger.reset(new spdlog::logger("cbwd", {console_sink}));

  logger->flush_on(spdlog::level::trace);

  std::set<std::string> to_update;
  std::set<std::string> to_delete;
  std::set<std::string> to_create;

  // Old configs that aren't present in the new should be deleted.
  // Old configs that are present in the new should be updated
  // or deleted/recreated.
  for (auto it = config.get_instances_configuration().begin(),
            end = config.get_instances_configuration().end();
       it != end; ++it) {
    instance_configuration new_config(
        cfg.get_instance_configuration(it->first));
    if (new_config.is_empty())
      to_delete.insert(it->first);
    else if (!it->second.same_child(new_config)) {
      to_delete.insert(it->first);
      to_create.insert(it->first);
    } else
      to_update.insert(it->first);
  }

  // Delete old processes.
  for (auto it = to_delete.begin(), end = to_delete.end(); it != end; ++it) {
    auto found = instances.find(*it);
    if (found != instances.end()) {
      instance* to_remove{found->second};
      instances.erase(found);
      delete to_remove;
    }
  }

  // New configs that aren't present in the old should be created.
  for (configuration::instance_map::const_iterator
           it(cfg.get_instances_configuration().begin()),
       end(cfg.get_instances_configuration().end());
       it != end; ++it)
    if (!config.instance_exists(it->first))
      to_create.insert(it->first);

  // Update processes.
  for (std::set<std::string>::const_iterator it(to_update.begin()),
       end(to_update.end());
       it != end; ++it) {
    std::unordered_map<std::string, instance*>::iterator found(
        instances.find(*it));
    if (found != instances.end()) {
      found->second->merge_configuration(cfg.get_instance_configuration(*it));
      found->second->update();
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
 *  @brief Signal handler
 *
 *  There is not many things we can do in signal handlers.
 *  We convert the signal into something usable by using the
 *  writing to a self-pipe pattern.
 *
 *  @param sig  The signal.
 */
static void signal_handler(int sig) {
  if (sig == SIGTERM || sig == SIGINT) {
    // Sigterm : should exit now...
    should_exit = true;
  } else if (sig == SIGHUP) {
    sighup = true;
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
  if (::sigaction(SIGTERM, &sig, nullptr) < 0 ||
      ::sigaction(SIGINT, &sig, nullptr) < 0 ||
      ::sigaction(SIGHUP, &sig, nullptr) < 0)
    throw msg_fmt("can't set the signal handlers");
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

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::warn);
  console_sink->set_pattern("[cbwd] [%^%l%$] %v");
  logger.reset(new spdlog::logger("cbwd", {console_sink}));
  logger->flush_on(spdlog::level::trace);

  if (argc != 2 || ::strcmp(argv[1], "-h") == 0) {
    print_help();
    return 0;
  }

  config_filename = argv[1];

  configuration config;

  try {
    configuration_parser parser;
    config = parser.parse(config_filename);
  } catch (std::exception const& e) {
    logger->error("Could not parse the configuration file '{}': {}",
                  config_filename, e.what());
    return 2;
  }

  int retval{0};

  // Create the main event loop.
  try {
    set_signal_handlers();
    apply_new_configuration(config);
    while (!should_exit) {
      int timeout = 5;
      int freq = 0;

      int status, stopped_pid;
      stopped_pid = waitpid(0, &status, WNOHANG);
      if (stopped_pid > 0) {
        logger->error("cbd instance with PID {} has stopped, attempt to restart it",
            stopped_pid);
        for (std::unordered_map<std::string, instance*>::iterator
                 it = instances.begin(),
                 end = instances.end();
             it != end; ++it) {
          instance* inst{it->second};
          if (inst->get_pid() == stopped_pid)
            inst->restart();
        }

        /* We measure the frequency of potential restarts of cbd.
         * - The watchdog checks every 5s.
         * - When a cbd needs a restart, the timeout is set to 0.
         * - If we restart cbd too often, the timeout is set again to 5.
         */
        freq++;
        if (freq / instances.size() > 5) {
          logger->error(
              "cbd seems to stop too many times, you must look at "
              "its configuration. The watchdog loop is slow down");
        } else
          timeout = 0;
      }

      if (sighup) {
        // Sighup : should reload...
        configuration config;
        try {
          configuration_parser parser;
          config = parser.parse(config_filename);
          apply_new_configuration(config);
        } catch (std::exception const& e) {
          logger->error("Could not parse the new configuration: {}",
                        e.what());
        }
        sighup = false;
        continue;
      }

      /* Let's wait a little */
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
           it{instances.begin()},
       end{instances.end()};
       it != end; ++it) {
    it->second->stop();
  }
  return retval;
}
