/*
** Copyright 2009-2012 Merethis
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

#include <cerrno>
#include <clocale>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <QCoreApplication>
#include <QLibraryInfo>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
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
  logging::config(logging::high)
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
  logging::info(logging::high)
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
  // Initialization.
  config::applier::init();

  // Return value.
  int retval(0);

  // Qt application object.
  QCoreApplication app(argc, argv);

  try {
    // Check the command line.
    bool check(false);
    bool debug(false);
    bool help(false);
    bool version(false);
    if (argc >= 2) {
      for (int i(1); i < argc; ++i)
        if (!strcmp(argv[i], "-c"))
          check = true;
        else if (!strcmp(argv[i], "-d"))
          debug = true;
        else if (!strcmp(argv[i], "-h"))
          help = true;
        else if (!strcmp(argv[i], "-v"))
          version = true;
        else
          gl_mainconfigfile = argv[i];
    }

    // Apply default configuration.
    config::state default_state;
    {
      // Logging object.
      config::logger default_log;
      default_log.config(!help);
      default_log.debug(debug);
      default_log.error(!help);
      default_log.info(true);
      logging::level level;
      if (debug)
        level = logging::low;
      else if (check)
        level = logging::medium;
      else
        level = logging::high;
      default_log.level(level);
      default_log.name((check || version) ? "stdout" : "stderr");
      default_log.type(config::logger::standard);

      // Configuration object.
      default_state.loggers().push_back(default_log);

      // Apply configuration.
      config::applier::state::instance().apply(default_state, false);
    }

    // Check parameters requirements.
    if (help) {
      logging::info(logging::high) << "USAGE: " << argv[0]
        << " [-c] [-d] [-h] [-v] [<configfile>]";
      logging::info(logging::high) << "  -c  Check configuration file.";
      logging::info(logging::high) << "  -d  Enable debug mode.";
      logging::info(logging::high) << "  -h  Print this help.";
      logging::info(logging::high)
        << "  -v  Print Centreon Broker version.";
      logging::info(logging::high) << "Centreon Broker "
        << CENTREON_BROKER_VERSION;
      logging::info(logging::high) << "Copyright 2009-2012 Merethis";
      logging::info(logging::high) << "License GNU GPL " \
        "version 2 <http://gnu.org/licenses/gpl.html>";
      retval = 0;
    }
    else if (version) {
      logging::info(logging::high) << "Centreon Broker "
        << CENTREON_BROKER_VERSION;
      retval = 0;
    }
    else if (gl_mainconfigfile.isEmpty()) {
      logging::error(logging::high) << "USAGE: " << argv[0]
        << " [-c] [-d] [-h] [-v] [<configfile>]";
      retval = 1;
    }
    else {
      app.setApplicationName("Centreon Broker");
#if QT_VERSION >= 0x040400
      app.setApplicationVersion(CENTREON_BROKER_VERSION);
#endif // Qt >= 4.4.0
      app.setOrganizationDomain("merethis.com");
      app.setOrganizationName("Merethis");
      logging::info(logging::medium)
        << "Centreon Broker " << CENTREON_BROKER_VERSION;
      logging::info(logging::medium) << "Copyright 2009-2012 Merethis";
      logging::info(logging::medium) << "License GNU GPL " \
        "version 2 <http://gnu.org/licenses/gpl.html>";
#if QT_VERSION >= 0x040400
      logging::info(logging::low) << "PID: " << app.applicationPid();
#endif // Qt >= 4.4.0
      logging::info(logging::medium)
        << "Qt compilation version " << QT_VERSION_STR;
      logging::info(logging::medium)
        << "Qt runtime version " << qVersion();
      logging::info(logging::medium) << "  Build Key: "
        << QLibraryInfo::buildKey();
      logging::info(logging::medium) << "  Licensee: "
        << QLibraryInfo::licensee();
      logging::info(logging::medium) << "  Licensed Products: "
        << QLibraryInfo::licensedProducts();

      // Reset locale.
      setlocale(LC_NUMERIC, "C");

      {
        // Parse configuration file.
        config::parser parsr;
        config::state conf;
        parsr.parse(gl_mainconfigfile, conf);

        // Verification modifications.
        if (check) {
          // Loggers.
          for (QList<config::logger>::iterator
                 it(conf.loggers().begin()),
                 end(conf.loggers().end());
               it != end;
               ++it)
            it->types(0);
          conf.loggers().push_back(default_state.loggers().front());
        }

        // Apply resulting configuration totally or partially.
        config::applier::state::instance().apply(conf, !check);
      }

      // Set configuration update handler.
      if (signal(SIGHUP, hup_handler) == SIG_ERR) {
        char const* err(strerror(errno));
        logging::info(logging::high)
          << "main: could not register configuration update handler: "
          << err;
      }

      // Set termination handler.
      if (signal(SIGTERM, term_handler) == SIG_ERR) {
        char const* err(strerror(errno));
        logging::info(logging::high)
          << "main: could not register termination handler: "
          << err;
      }

      // Launch event loop.
      if (!check)
        retval = app.exec();
      else
        retval = EXIT_SUCCESS;
    }
  }
  // Standard exception.
  catch (std::exception const& e) {
    logging::error(logging::high) << e.what();
    retval = EXIT_FAILURE;
  }
  // Unknown exception.
  catch (...) {
    logging::error(logging::high)
      << "main: unknown error, aborting execution";
    retval = EXIT_FAILURE;
  }

  // Unload endpoints.
  config::applier::deinit();

  return (retval);
}
