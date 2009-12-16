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

#include <iostream>                         // for cerr
#ifdef USE_MYSQL
# include <mysql.h>                         // for mysql_library_init
#endif /* USE_MYSQL */
#include <signal.h>
#include "concurrency/condition_variable.h"
#include "concurrency/lock.h"
#include "concurrency/mutex.h"
#include "configuration/manager.h"
#include "exception.h"
#include "interface/ndo/source.h"
#include "interface/xml/destination.h"
#include "logging.h"
#include "mapping.h"

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Condition variable used to wake up the main thread when receiving SIGTERM.
static Concurrency::ConditionVariable gl_cv;
// Mutex linked to the condition variable above.
static Concurrency::Mutex             gl_mutex;
// Determines whether or not SIGTERM has been received.
static bool                           gl_exit = false;
// Original SIGTERM handler.
static void (*                        gl_sigterm_handler)(int) = NULL;

/**
 *  Function called on termination request (when program receives SIGTERM).
 *  \par Safety No throw guarantee.
 *
 *  \param[in] signum Signal number.
 */
static void term_handler(int signum)
{
  try
    {
      Concurrency::Lock lock(gl_mutex);

      // Signal number is ignored.
      (void)signum;
      // Main thread should exit.
      gl_exit = true;
      // Wake up main thread.
      gl_cv.WakeAll();
      // Restore original signal handler.
      signal(SIGTERM, gl_sigterm_handler);
    }
  catch (const std::exception& e)
    {
      LOGERROR(e.what());
    }
  catch (...)
    {
      LOGERROR("Unknown exception occured while " \
               "executing termination handler.");
    }
  return ;
}

/**************************************
*                                     *
*          Public Functions           *
*                                     *
**************************************/

/**
 *  \brief Program entry point.
 *
 *  main() is the first function called when the program starts.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] argc Number of arguments received on the command line.
 *  \param[in] argv Arguments received on the command line, stored in an array.
 *
 *  \return 0 on normal termination, any other value on failure.
 */
int main(int argc, char* argv[])
{
  int exit_code;

  try
    {
      // Check the command line.
      if (argc != 2)
        {
          std::cerr << "USAGE: " << argv[0] << " <configfile>" << std::endl;
          exit_code = 1;
        }
      else
        {
          // Lock termination mutex.
          Concurrency::Lock lock(gl_mutex);

#ifdef USE_MYSQL
          // Initialize MySQL library.
          LOGDEBUG("Initializing MySQL library ...");
          if (mysql_library_init(0, NULL, NULL))
            throw (Exception(0, "MySQL library initialization failed."));
#endif /* USE_MYSQL */

          // Initialize all interface objects.
	  LOGDEBUG("Initializing DB engine (destination) ...");
	  MappingsInit();
          LOGDEBUG("Initializing NDO engine (source) ...");
          Interface::NDO::Source::Initialize();
          LOGDEBUG("Initializing XML engine (destination) ...");
          Interface::XML::Destination::Initialize();

          // Load configuration file.
          Configuration::Manager::Instance().Open(argv[1]);

          // Register handler for SIGTERM.
          gl_sigterm_handler = signal(SIGTERM, term_handler);
          if (SIG_ERR == gl_sigterm_handler)
            LOGINFO("Unable to register termination handler. Unpredictable " \
                    "behavior will occur when exiting.");

          // Everything's loaded, sleep until we have to exit.
          while (!gl_exit)
            gl_cv.Sleep(gl_mutex);

          // Unload configuration.
          Configuration::Manager::Instance().Close();

	  // Destroy O/R mapping.
	  LOGDEBUG("Unloading DB engine ...");
	  MappingsDestroy();

#ifdef USE_MYSQL
          // Unload MySQL library.
          LOGDEBUG("Unloading MySQL library ...");
          mysql_library_end();
#endif /* USE_MYSQL */

          // Everything went well.
          exit_code = 0;
        }
    }
  // Standard exception.
  catch (const std::exception& e)
    {
      std::cerr << "Caught exception :" << std::endl
                << '\t' << e.what() << std::endl;
      exit_code = 1;
    }
  // Unknown exception.
  catch (...)
    {
      std::cerr << "Caught unknown exception, aborting execution ..."
                << std::endl;
      exit_code = 1;
    }
  return (exit_code);
}
