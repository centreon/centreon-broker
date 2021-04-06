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

#include <getopt.h>

#include <cerrno>
#include <chrono>
#include <clocale>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <thread>

#include "com/centreon/broker/brokerrpc.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/config/applier/logger.hh"
#include "com/centreon/broker/config/applier/state.hh"
#include "com/centreon/broker/config/logger.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/config/state.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/diagnostic.hh"

using namespace com::centreon::broker;

// Main config file.
static std::vector<std::string> gl_mainconfigfiles;
static config::state gl_state;
static std::atomic_bool gl_term{false};

static struct option long_options[] = {{"pool_size", required_argument, 0, 's'},
                                       {"check", no_argument, 0, 'c'},
                                       {"debug", no_argument, 0, 'd'},
                                       {"diagnose", no_argument, 0, 'D'},
                                       {"version", no_argument, 0, 'v'},
                                       {"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};

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
  log_v2::core()->info("main: configuration update requested");
  logging::config(logging::high) << "main: configuration update requested";

  try {
    // Parse configuration file.
    config::parser parsr;
    config::state conf{parsr.parse(gl_mainconfigfiles.front())};
    try {
      log_v2::instance().apply(conf);
    } catch (const std::exception& e) {
      log_v2::core()->error(e.what());
    }

    try {
      // Apply resulting configuration.
      config::applier::state::instance().apply(conf);

      gl_state = conf;
    } catch (const std::exception& e) {
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
  } catch (const std::exception& e) {
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
static void term_handler(int signum) {
  (void)signum;
  gl_term = true;
}

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
  int opt, option_index = 0, n_thread = 0;
  std::string broker_name{"unknown"};
  uint16_t default_port{51000};

  // Return value.
  int retval(0);

  try {
    // Check the command line.
    bool check(false);
    bool debug(false);
    bool diagnose(false);
    bool help(false);
    bool version(false);

    opt = getopt_long(argc, argv, "t:cdDvh", long_options, &option_index);
    switch (opt) {
      case 't':
        n_thread = atoi(optarg);
        break;
      case 'c':
        check = true;
        break;
      case 'd':
        debug = true;
        break;
      case 'D':
        diagnose = true;
        break;
      case 'h':
        help = true;
        break;
      case 'v':
        version = true;
        break;
      default:
        break;
    }

    if (optind < argc)
      while (optind < argc)
        gl_mainconfigfiles.push_back(argv[optind++]);

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
          << "USAGE: " << argv[0]
          << " [-t] [-c] [-d] [-D] [-h] [-v] [<configfile>]";
      logging::info(logging::high) << "  -t  Set x threads.";
      logging::info(logging::high) << "  -c  Check configuration file.";
      logging::info(logging::high) << "  -d  Enable debug mode.";
      logging::info(logging::high) << "  -D  Generate a diagnostic file.";
      logging::info(logging::high) << "  -h  Print this help.";
      logging::info(logging::high) << "  -v  Print Centreon Broker version.";
      logging::info(logging::high)
          << "Centreon Broker " << CENTREON_BROKER_VERSION;
      logging::info(logging::high) << "Copyright 2009-2021 Centreon";
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
          << "USAGE: " << argv[0]
          << " [-c] [-d] [-D] [-h] [-v] [<configfile>]\n\n";
      return 1;
    } else {
      logging::info(logging::medium)
          << "Centreon Broker " << CENTREON_BROKER_VERSION;
      logging::info(logging::medium) << "Copyright 2009-2021 Centreon";
      logging::info(logging::medium)
          << "License ASL 2.0 "
             "<http://www.apache.org/licenses/LICENSE-2.0>";

      // Reset locale.
      setlocale(LC_NUMERIC, "C");

      {
        // Parse configuration file.
        config::parser parsr;
        config::state conf{parsr.parse(gl_mainconfigfiles.front())};
        try {
          log_v2::instance().apply(conf);
        } catch (const std::exception& e) {
          log_v2::core()->error(e.what());
        }

        if (n_thread > 0 && n_thread < 100)
          conf.pool_size(n_thread);
        config::applier::init(conf);

        // Verification modifications.
        if (check) {
          // Loggers.
          for (auto& l : conf.loggers())
            l.types(0);
          conf.loggers().push_back(default_state.loggers().front());
        }

        // Add debug output if in debug mode.
        if (debug)
          conf.loggers().insert(conf.loggers().end(),
                                default_state.loggers().begin(),
                                default_state.loggers().end());

        // Apply resulting configuration totally or partially.
        config::applier::state::instance().apply(conf, !check);
        broker_name = conf.broker_name();
        gl_state = conf;
      }

      // Set configuration update handler.
      if (signal(SIGHUP, hup_handler) == SIG_ERR) {
        char const* err{strerror(errno)};
        logging::info(logging::high)
            << "main: could not register configuration update handler: " << err;
      }

      // Init signal handler.
      struct sigaction sigterm_act;
      memset(&sigterm_act, 0, sizeof(sigterm_act));
      sigterm_act.sa_handler = &term_handler;

      // Set termination handler.
      if (sigaction(SIGTERM, &sigterm_act, nullptr) < 0)
        logging::info(logging::high)
            << "main: could not register termination handler";

      if (gl_state.rpc_port() == 0)
        default_port += gl_state.broker_id();
      else
        default_port = gl_state.rpc_port();
      std::unique_ptr<brokerrpc, std::function<void(brokerrpc*)> > rpc(
          new brokerrpc("0.0.0.0", default_port, broker_name),
          [](brokerrpc* rpc) {
            rpc->shutdown();
            delete rpc;
          });

      // Launch event loop.
      retval = EXIT_SUCCESS;
      if (!check) {
        while (!gl_term) {
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        log_v2::core()->info("main: termination request received by process");
      }
      // Unload endpoints.
      config::applier::deinit();
    }
  }
  // Standard exception.
  catch (const std::exception& e) {
    log_v2::core()->error("Error during cbd exit: {}", e.what());
    logging::error(logging::high) << e.what();
    retval = EXIT_FAILURE;
  }
  // Unknown exception.
  catch (...) {
    log_v2::core()->error("Error general during cbd exit");
    logging::error(logging::high) << "main: unknown error, aborting execution";
    retval = EXIT_FAILURE;
  }

  return retval;
}
