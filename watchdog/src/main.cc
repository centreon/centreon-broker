/*
** Copyright 2015 Centreon
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

#include <csignal>
#include <cstring>
#include <iostream>
#include "com/centreon/broker/watchdog/configuration_parser.hh"

static char const* help_msg =
    "usage: cbwd configuration_file";

/**
 *  Contain the configuration file given in argument.
 */
static char* config_filename = NULL;

/**
 *  Print the help.
 */
static void print_help() {
  std::cout << help_msg << std::endl;
}

/**
 *  Signal handler
 *
 *  @param sig
 */
static void signal_handler(int sig) {
  if (sig == SIGTERM || sig == SIGINT) {
    ;
  }
  else if (sig == SIGHUP) {
    ;
  }
}


/**
 *  Centreon Watchdog entry point.
 *
 *  @param[in] argc  Parameter count.
 *  @param[in] argv  Parameter values.
 *
 *  @return  0 on success.
 */
int main(int argc, char **argv) {
  // Check arguments.
  if (argc != 2 || ::strcmp(argv[1], "-h") == 0) {
    print_help();
    return (0);
  }
  config_filename = argv[1];

  // Parse the configuration.
  com::centreon::broker::watchdog::configuration_parser
    parser;
  com::centreon::broker::watchdog::configuration
    config;

  try {
    config = parser.parse(config_filename);
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    return (-1);
  }

  // Add signal manager.
  {
    struct sigaction sig;
    sig.sa_handler = signal_handler;
    ::sigemptyset(&sig.sa_mask);
    ::sigfillset(&sig.sa_mask);
    sig.sa_flags = 0;
    if (::sigaction(SIGTERM, &sig, NULL) < 0
        || ::sigaction(SIGINT, &sig, NULL) < 0
        || ::sigaction(SIGHUP, &sig, NULL) < 0) {
      std::cerr << "can't set signal handlers" << std::endl;
      return (-1);
    }
  }

  return (0);
}
