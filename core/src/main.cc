/*
** Copyright 2009-2013,2015,2018 Centreon
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

#include <cerrno>
#include <chrono>
#include <clocale>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <thread>
#include "com/centreon/broker/config/applier/endpoint.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/modules.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/diagnostic.hh"

using namespace com::centreon::broker;

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Main config file.
static std::vector<std::string> gl_mainconfigfiles;
static config::state gl_state;

/**
 *  Function called when updating configuration (when program receives
 *  SIGHUP).
 *
 *  @param[in] signum Signal number.
 */
static void hup_handler(int signum) {
  (void)signum;

  // Disable SIGHUP handling during handler execution.
  signal(SIGHUP, SIG_IGN);

  // Log message.
  logging::config(logging::high) << "main: configuration update requested";

  try {
    // Parse configuration file.
    config::parser parsr;
    config::state conf{parsr.parse(gl_mainconfigfiles.front())};

    try {
      // Apply resulting configuration.
      config::applier::state::instance().apply(conf);
      gl_state = conf;
    } catch (std::exception const& e) {
      logging::error(logging::high)
          << "main: configuration update "
          << "could not succeed, reloading previous configuration: "
          << e.what();
      config::applier::state::instance().apply(gl_state);
    } catch (...) {
      logging::error(logging::high)
          << "main: configuration update "
          << "could not succeed, reloading previous configuration";
      config::applier::state::instance().apply(gl_state);
    }
  } catch (std::exception const& e) {
    logging::config(logging::high)
        << "main: configuration update failed: " << e.what();
  } catch (...) {
    logging::config(logging::high)
        << "main: configuration update failed: unknown exception";
  }

  // Reenable SIGHUP handler.
  signal(SIGHUP, &hup_handler);
}

/**
 *  Function called on termination request (when program receives
 *  SIGTERM).
 *
 *  @param[in] signum Unused.
 *  @param[in] info   Signal informations.
 *  @param[in] data   Unused.
 */
static void term_handler(int signum, siginfo_t* info, void* data) {
  (void)signum;
  (void)data;

  // Log message.
  logging::info(logging::high)
      << "main: termination request received by process id " << info->si_pid
      << " with real user id " << info->si_uid;

  // Ask event loop to quit.
  // FIXME DBR
  exit(0);
  // QCoreApplication::exit(0);
}

#ifdef COVERAGE
void __gcov_flush(void);

void dump_coverage(int signum __attribute__((unused))) {
  /* dump coverage data on receiving SIGUSR1 */
  __gcov_flush();
  exit(0);
}
#endif

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
  // QCoreApplication app(argc, argv);
  // QTextCodec* utf8_codec(QTextCodec::codecForName("UTF-8"));
  // if (utf8_codec)
  //  QTextCodec::setCodecForCStrings(utf8_codec);
  // else
  //  logging::error(logging::high)
  //    << "core: could not find UTF-8 codec, strings will be "
  //       "interpreted using the current locale";

  try {
    // Check the command line.
    bool check(false);
    bool debug(false);
    bool diagnose(false);
    bool help(false);
    bool version(false);
    if (argc >= 2) {
      for (int i(1); i < argc; ++i)
        if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--check"))
          check = true;
        else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug"))
          debug = true;
        else if (!strcmp(argv[i], "-D") || !strcmp(argv[i], "--diagnose"))
          diagnose = true;
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
          help = true;
        else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version"))
          version = true;
        else
          gl_mainconfigfiles.push_back(argv[i]);
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
      config::applier::logger::instance().apply(default_state.loggers());
    }

    // Check parameters requirements.
    if (diagnose) {
      if (gl_mainconfigfiles.empty()) {
        logging::error(logging::high)
            << "diagnostic: no configuration file provided: "
            << "DIAGNOSTIC FILE MIGHT NOT BE USEFUL";
      }
      misc::diagnostic diag;
      diag.generate(gl_mainconfigfiles);
    } else if (help) {
      logging::info(logging::high)
          << "USAGE: " << argv[0] << " [-c] [-d] [-D] [-h] [-v] [<configfile>]";
      logging::info(logging::high) << "  -c  Check configuration file.";
      logging::info(logging::high) << "  -d  Enable debug mode.";
      logging::info(logging::high) << "  -D  Generate a diagnostic file.";
      logging::info(logging::high) << "  -h  Print this help.";
      logging::info(logging::high) << "  -v  Print Centreon Broker version.";
      logging::info(logging::high)
          << "Centreon Broker " << CENTREON_BROKER_VERSION;
      logging::info(logging::high) << "Copyright 2009-2018 Centreon";
      logging::info(logging::high)
          << "License ASL 2.0 "
             "<http://www.apache.org/licenses/LICENSE-2.0>";
      retval = 0;
    } else if (version) {
      logging::info(logging::high)
          << "Centreon Broker " << CENTREON_BROKER_VERSION;
      retval = 0;
    } else if (gl_mainconfigfiles.empty()) {
      logging::error(logging::high)
          << "USAGE: " << argv[0] << " [-c] [-d] [-D] [-h] [-v] [<configfile>]";
      retval = 1;
    } else {
      //      app.setApplicationName("Centreon Broker");
      //#if QT_VERSION >= 0x040400
      //      app.setApplicationVersion(CENTREON_BROKER_VERSION);
      //#endif // Qt >= 4.4.0
      //      app.setOrganizationDomain("centreon.com");
      //      app.setOrganizationName("Centreon");
      logging::info(logging::medium)
          << "Centreon Broker " << CENTREON_BROKER_VERSION;
      logging::info(logging::medium) << "Copyright 2009-2018 Centreon";
      logging::info(logging::medium)
          << "License ASL 2.0 "
             "<http://www.apache.org/licenses/LICENSE-2.0>";
      //#if QT_VERSION >= 0x040400
      //      logging::info(logging::low) << "PID: " << app.applicationPid();
      //#endif // Qt >= 4.4.0
      //      logging::info(logging::medium)
      //        << "Qt compilation version " << QT_VERSION_STR;
      //      logging::info(logging::medium)
      //        << "Qt runtime version " << qVersion();
      //      logging::info(logging::medium) << "  Build Key: "
      //        << QLibraryInfo::buildKey();
      //      logging::info(logging::medium) << "  Licensee: "
      //        << QLibraryInfo::licensee();
      //      logging::info(logging::medium) << "  Licensed Products: "
      //        << QLibraryInfo::licensedProducts();

      // Reset locale.
      setlocale(LC_NUMERIC, "C");

      {
        // Parse configuration file.
        config::parser parsr;
        config::state conf{parsr.parse(gl_mainconfigfiles.front())};

        // Verification modifications.
        if (check) {
          // Loggers.
          for (std::list<config::logger>::iterator it(conf.loggers().begin()),
               end(conf.loggers().end());
               it != end; ++it)
            it->types(0);
          conf.loggers().push_back(default_state.loggers().front());
        }

        // Add debug output if in debug mode.
        if (debug)
          conf.loggers().insert(conf.loggers().end(),
                                default_state.loggers().begin(),
                                default_state.loggers().end());

        // Apply resulting configuration totally or partially.
        config::applier::state::instance().apply(conf, !check);
        gl_state = conf;
      }

#ifdef COVERAGE
      struct sigaction new_action, old_action;

      new_action.sa_handler = dump_coverage;
      sigemptyset(&new_action.sa_mask);
      new_action.sa_flags = 0;

      sigaction(SIGUSR1, nullptr, &old_action);
      if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGUSR1, &new_action, nullptr);
#endif

      // Set configuration update handler.
      if (signal(SIGHUP, hup_handler) == SIG_ERR) {
        char const* err(strerror(errno));
        logging::info(logging::high)
            << "main: could not register configuration update handler: " << err;
      }

      // Init signal handler.
      struct sigaction sigterm_act;
      memset(&sigterm_act, 0, sizeof(sigterm_act));
      sigterm_act.sa_sigaction = &term_handler;
      sigterm_act.sa_flags = SA_SIGINFO | SA_RESETHAND;

      // Set termination handler.
      if (sigaction(SIGTERM, &sigterm_act, nullptr) < 0)
        logging::info(logging::high)
            << "main: could not register termination handler";

      // Launch event loop.
      if (!check)
        while (true) {
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
      // retval = app.exec();
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
    logging::error(logging::high) << "main: unknown error, aborting execution";
    retval = EXIT_FAILURE;
  }

  // Unload endpoints.
  config::applier::deinit();

  return retval;
}
