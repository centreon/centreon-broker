/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <csignal>
#ifdef USE_TLS
# include <gnutls/gnutls.h>
#endif /* USE_TLS */
#include <iostream>
#ifdef USE_MYSQL
# include <mysql.h>
#endif /* USE_MYSQL */
#ifdef USE_ORACLE
# include <ocilib.h>
#endif /* USE_ORACLE */
#include "conf/manager.h"
#include "exception.h"
#include "logging.h"
#include "mapping.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Condition variable to wake up the main thread when receiving SIGTERM.
static boost::condition_variable gl_cv;
// Mutex linked to the condition variable above.
static boost::mutex              gl_mutex;
// This global boolean determines whether or the main thread should exit.
static volatile bool             gl_shall_exit = false;
// Original SIGTERM handler.
static void (*                   gl_sigterm_handler)(int) = NULL;

/**
 *  This function is called on termination request (when program receives
 *  SIGTERM).
 *
 *  \param[in] signum Signal number (ignored).
 */
static void term_handler(int signum)
{
  boost::unique_lock<boost::mutex> lock(gl_mutex);

  (void)signum;

  // Wake up main thread.
  gl_shall_exit = true;
  gl_cv.notify_all();

  // Restore original signal handler.
  signal(SIGTERM, gl_sigterm_handler);

  return ;
}

/**
 *  Program entry point.
 *
 *  \param[in] argc Number of arguments received on the command line.
 *  \param[in] argv Arguments received on the command line, stored in an array.
 *
 *  \return EXIT_SUCCESS on normal termination, any other value on failure.
 */
int main(int argc, char* argv[])
{
  int exit_code;

  if (argc != 2)
    {
      std::cout << "USAGE: " << argv[0] << " <configfile>" << std::endl;
      exit_code = EXIT_FAILURE;
    }
  else
    {
      try
        {
          logging.LogInfo("Starting CentreonBroker initialization ...");

          // Lock main mutex
          boost::unique_lock<boost::mutex> lock(gl_mutex);

          // Initialize MySQL library if supported
#ifdef USE_MYSQL
# ifndef NDEBUG
          logging.LogDebug("Initializing MySQL library ...");
# endif /* !NDEBUG */
          if (mysql_library_init(0, NULL, NULL))
            throw (Exception(0, "MySQL library initialization failed."));
#endif /* USE_MYSQL */


          // Initialize OCILIB if supported
#ifdef USE_ORACLE
# ifndef NDEBUG
          logging.LogDebug("Initializing OCILIB library ...");
# endif /* !NDEBUG */
          if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
            throw (Exception(0, "OCILIB library initialization failed."));
#endif /* USE_ORACLE */


          // Initialize GNU TLS library if supported
#ifdef USE_TLS
# ifndef NDEBUG
          logging.LogDebug("Initializing GNU TLS library...");
# endif /* !NDEBUG */
          if (gnutls_global_init() != GNUTLS_E_SUCCESS)
            throw (Exception(0, "GNU TLS library initialization failed."));
#endif /* USE_TLS */

          // Load Object-Relational mappings
          MappingsInit();

          // Load configuration file
          Conf::Manager::GetInstance().Open(argv[1]);

          // Register handler for SIGTERM
          gl_sigterm_handler = signal(SIGTERM, term_handler);
          if (gl_sigterm_handler == SIG_ERR)
            logging.LogInfo("Unable to register termination handler, " \
                            "unpredictable behavior will occur when "  \
                            "exiting.");

          // Everything's loaded, sleep until we have to exit
          logging.LogInfo("Initialization completed successfully !");
          while (!gl_shall_exit)
            gl_cv.wait(lock);

          // Unload everything
          Conf::Manager::GetInstance().Close();

          // Unload Object-Relational mappings
          MappingsDestroy();


          // Unload GNU TLS library
#ifdef USE_TLS
# ifndef NDEBUG
          logging.LogDebug("Unloading GNU TLS library ...");
# endif /* !NDEBUG */
          gnutls_global_deinit();
#endif /* USE_TLS */


          // Unload OCILIB library
#ifdef USE_ORACLE
# ifndef NDEBUG
          logging.LogDebug("Unloading OCILIB library ...");
# endif /* !NDEBUG */
          OCI_Cleanup();
#endif /* USE_ORACLE */


          // Unload MySQL library
#ifdef USE_MYSQL
# ifndef NDEBUG
          logging.LogDebug("Unloading MySQL library ...");
# endif /* !NDEBUG */
          mysql_library_end();
#endif /* USE_MYSQL */


          // Everything went well
          exit_code = EXIT_SUCCESS;
        }
      catch (std::exception& e)
        {
          logging.LogInfo("Caught exception in main(), " \
                          "see error message below.");
          logging.LogInfo(e.what());
          exit_code = EXIT_FAILURE;
        }
      catch (...)
        {
          logging.LogInfo("Caught unknown exception in main().");
          exit_code = EXIT_FAILURE;
        }

#ifndef NDEBUG
      logging.LogDebug("Exiting main()");
#endif /* !NDEBUG */
    }
  return (exit_code);
}
