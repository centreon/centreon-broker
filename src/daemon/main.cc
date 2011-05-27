/*
** Copyright 2009-2011 MERETHIS
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

#include <errno.h>
#include <QCoreApplication>
#include <signal.h>
#include <string.h>
#include "config/applier/state.hh"
#include "config/logger.hh"
#include "config/parser.hh"
#include "config/state.hh"
#include "exceptions/basic.hh"
#include "logging/logging.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Function called on termination request (when program receives
 *  SIGTERM).
 *
 *  @param[in] signum Signal number.
 */
static void term_handler(int signum) {
  (void)signum;

  // Log message.
  logging::info << logging::HIGH << "termination request received";

  // Reset original signal handler.
  signal(SIGTERM, SIG_DFL);

  // Ask event loop to quit.
  QCoreApplication::exit(0);

  return ;
}

/**************************************
*                                     *
*          Public Functions           *
*                                     *
**************************************/

/**
 *  @brief Program entry point.
 *
 *  main() is the first function called when the program starts.
 *
 *  @param[in] argc Number of arguments received on the command line.
 *  @param[in] argv Arguments received on the command line, stored in an
 *                  array.
 *
 *  @return 0 on normal termination, any other value on failure.
 */
int main(int argc, char* argv[]) {
  int exit_code;

  try {
    // Apply default configuration (log important messages on stderr).
    {
      // Logging object.
      config::logger default_log;
      default_log.config(true);
      default_log.debug(false);
      default_log.error(true);
      default_log.info(true);
      default_log.level(logging::HIGH);
      default_log.name("stderr");
      default_log.type(config::logger::standard);

      // Configuration object.
      config::state default_state;
      default_state.loggers().push_back(default_log);

      // Apply configuration.
      config::applier::state::instance().apply(default_state);
    }

    // Check the command line.
    if (argc != 2) {
      logging::error << logging::HIGH << "USAGE: " << argv[0]
                     << " <configfile>";
      exit_code = 1;
    }
    else {
      // Initialize QCoreApplication object.
      QCoreApplication app(argc, argv);
      app.setApplicationName("Centreon Broker");
      app.setApplicationVersion("2");
      app.setOrganizationDomain("merethis.com");
      app.setOrganizationName("Merethis");

      {
        // Parse configuration file.
        config::parser parsr;
        config::state conf;
        parsr.parse(argv[1], conf);

        // Apply resulting configuration.
        config::applier::state::instance().apply(conf);
      }

      // Set termination handler.
      if (signal(SIGTERM, term_handler) == SIG_ERR)
        logging::info << logging::HIGH
          << "could not register termination handler: "
          << strerror(errno);

      // Launch event loop.
      exit_code = app.exec();
    }
  }
  // Standard exception.
  catch (std::exception const& e) {
    logging::error << logging::HIGH << e.what();
    exit_code = 1;
  }
  // Unknown exception.
  catch (...) {
    logging::error << logging::HIGH << "unknown error, stopping execution";
    exit_code = 1;
  }

  return (exit_code);
}
