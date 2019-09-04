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

#include <unistd.h>
#include <QCoreApplication>
#include <csignal>
#include <cstring>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/watchdog/application.hh"
#include "com/centreon/broker/watchdog/configuration_parser.hh"

#include <QMetaObject>

static com::centreon::broker::watchdog::application* p_app = NULL;

static char const* help_msg = "USAGE: cbwd configuration_file";

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
  if (!p_app)
    return;

  if (sig == SIGTERM || sig == SIGINT) {
    char a = 1;
    ::write(com::centreon::broker::watchdog::application::sigterm_fd[0], &a,
            sizeof(a));
  } else if (sig == SIGHUP) {
    char a = 1;
    ::write(com::centreon::broker::watchdog::application::sighup_fd[0], &a,
            sizeof(a));
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
    throw(com::centreon::broker::exceptions::msg()
          << "can't set the signal handlers");
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
  // Create QApplication.
  QCoreApplication a(argc, argv);

  // Check arguments.
  if (argc != 2 || ::strcmp(argv[1], "-h") == 0) {
    print_help();
    return (0);
  }
  char* config_filename = argv[1];

  // Create the main even loop.
  try {
    com::centreon::broker::watchdog::application app(config_filename);
    p_app = &app;
    set_signal_handlers();
    app.exec();
  } catch (std::exception const& e) {
    std::cerr << "watchdog: " << e.what() << std::endl;
    return (1);
  }

  return (0);
}
