/*
** Copyright 2009-2011 Merethis
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
#include <exception>
#include <locale.h>
#include <QCoreApplication>
#include <signal.h>
#include <string.h>
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Main config file.
static QString gl_mainconfigfile;

/**
 *  Function called when updating configuration (when program receives
 *  SIGHUP).
 *
 *  @param[in] signum Signal number.
 */
static void hup_handler(int signum) {
  (void)signum;

  // Log message.
  logging::config << logging::HIGH
    << "main: configuration update requested";

  // Parse configuration file.
  config::parser parsr;
  config::state conf;
  parsr.parse(gl_mainconfigfile, conf);

  // Apply resulting configuration.
  config::applier::state::instance().apply(conf);

  return ;
}

/**
 *  Function called on termination request (when program receives
 *  SIGTERM).
 *
 *  @param[in] signum Signal number.
 */
static void term_handler(int signum) {
  (void)signum;

  // Log message.
  logging::info << logging::HIGH
    << "main: termination request received";

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

      // Reset locale.
      setlocale(LC_NUMERIC, "C");

      {
        // Set main configuration file.
        gl_mainconfigfile = argv[1];

        // Parse configuration file.
        config::parser parsr;
        config::state conf;
        parsr.parse(gl_mainconfigfile, conf);

        // Apply resulting configuration.
        config::applier::state::instance().apply(conf);
      }

      // Set configuration update handler.
      if (signal(SIGHUP, hup_handler) == SIG_ERR) {
        char const* err(strerror(errno));
        logging::info << logging::HIGH
          << "main: could not register configuration update handler: "
          << err;
      }

      // Set termination handler.
      if (signal(SIGTERM, term_handler) == SIG_ERR) {
        char const* err(strerror(errno));
        logging::info << logging::HIGH
          << "main: could not register termination handler: "
          << err;
      }

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
    logging::error << logging::HIGH
      << "main: unknown error, stopping execution";
    exit_code = 1;
  }

  return (exit_code);
}
