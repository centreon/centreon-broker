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
**
** For more information: contact@centreon.com
*/

#include <iostream>
#include <signal.h>
#include <time.h>
#include "concurrency/condition_variable.hh"
#include "concurrency/lock.hh"
#include "concurrency/mutex.hh"
#include "config/handle.hh"
#include "init.hh"
#include "logging/logging.hh"
#include "logging/ostream.hh"
#include "mapping.hh"

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Condition variable used to wake up the main thread when receiving SIGTERM.
static concurrency::condition_variable gl_cv;
// Mutex linked to the condition variable above.
static concurrency::mutex              gl_mutex;
// Determines whether or not SIGTERM has been received.
static bool                            gl_exit = false;
// Original SIGTERM handler.
static void (*                         gl_sigterm_handler)(int) = NULL;

/**
 *  Function called on termination request (when program receives
 *  SIGTERM).
 *
 *  @param[in] signum Signal number.
 */
static void term_handler(int signum) {
  logging::info << logging::HIGH << "caught termination signal";
  try {
    concurrency::lock l(gl_mutex);

    // Signal number is ignored.
    (void)signum;
    // Main thread should exit.
    gl_exit = true;
    // Wake up main thread.
    gl_cv.wake_all();
    // Restore original signal handler.
    signal(SIGTERM, gl_sigterm_handler);
  }
  catch (std::exception const& e) {
    logging::error << logging::HIGH
                   << "termination handler error: " << e.what();
  }
  catch (...) {
    logging::error << logging::HIGH
                   << "termination handler unknown error";
  }
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
    // Check the command line.
    if (argc != 2) {
      std::cerr << "USAGE: " << argv[0] << " <configfile>" << std::endl;
      exit_code = 1;
    }
    else {
      // Lock termination mutex.
      concurrency::lock l(gl_mutex);

      // Global initialization.
      init();

      // Initial logging object.
      logging::backend* b(new logging::ostream(std::cerr));
      logging::log_on(b, logging::CONFIG | logging::ERROR, logging::HIGH);

      // Load configuration file.
      config::handle(argv[1]);

      // Remove initial logging object.
      logging::log_on(b, 0, logging::NONE);

      // Register handler for SIGTERM.
      gl_sigterm_handler = signal(SIGTERM, term_handler);
      if (SIG_ERR == gl_sigterm_handler)
        logging::info << logging::MEDIUM
                      << "unable to register termination handler";

      // Everything's loaded, sleep for 60 seconds, then perform some
      // potential cleanup and sleep again.
      while (!gl_exit) {
	gl_cv.sleep(gl_mutex, time(NULL) + 60);
        config::reap();
      }

      // Global unloading.
      deinit();

      // Everything went well.
      exit_code = 0;
    }
  }
  // Standard exception.
  catch (std::exception const& e) {
    std::cerr << "Caught exception :" << std::endl
              << '\t' << e.what() << std::endl;
    exit_code = 1;
  }
  // Unknown exception.
  catch (...) {
    std::cerr << "Caught unknown exception, aborting execution ..."
              << std::endl;
    exit_code = 1;
  }
  return (exit_code);
}
