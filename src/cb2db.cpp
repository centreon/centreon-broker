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

#include <csignal>
#include <cstdlib>
#include <gnutls/gnutls.h>
#include <iostream>
#ifdef USE_MYSQL
# include <mysql.h>
#endif /* USE_MYSQL */
#include "conf/manager.h"
#include "exception.h"
#include "logging.h"
#include "mapping.h"

using namespace CentreonBroker;

static volatile bool gl_shall_exit = false;

/**
 *  This function is called when termination is requested.
 */
static void term_handler(int signum)
{
  (void)signum;
  gl_shall_exit = true;
  raise(SIGURG);
  signal(SIGINT, term_handler);
  return ;
}

/**
 *  Program entry point.
 */
int main(int argc, char* argv[])
{
  int exit_code;
  std::vector<DBOutput*> dbs;
  std::vector<ClientAcceptor*> sockets;

  if (argc != 2)
    {
      std::cout << "USAGE: " << argv[0] << " <configfile>" << std::endl;
      exit_code = 1;
    }
  else
    {
      try
        {
          logging.LogInfo("Starting initialization");
#ifndef NDEBUG
          logging.LogDebug("Initializing MySQL library...");
#endif /* !NDEBUG */
#ifdef USE_MYSQL
          mysql_library_init(0, NULL, NULL);
#endif /* USE_MYSQL */
#ifdef USE_TLS
	  if (gnutls_global_init() != GNUTLS_E_SUCCESS)
	    throw (Exception(0, "Initialization of GNU TLS failed."));
#endif /* USE_TLS */

          // Load Object-Relational mappings
          InitMappings();

          // Load configuration file
	  Conf::Manager::GetInstance().Open(argv[1]);

          // Catch ^C
          signal(SIGINT, term_handler);

          // Everything loaded, ready to go
          logging.LogInfo("Initialization completed, waiting for clients...");
          while (!gl_shall_exit)
	    pause();

	  // Unload everything
	  Conf::Manager::GetInstance().Close();

          exit_code = 0;
        }
      catch (std::exception& e)
        {
          logging.LogInfo("Process terminated because of this exception :");
          logging.LogInfo(e.what());
          exit_code = 1;
        }

#ifdef USE_TLS
      gnutls_global_deinit();
#endif /* USE_TLS */
#ifdef USE_MYSQL
      mysql_library_end();
#endif /* USE_MYSQL */

#ifndef NDEBUG
      logging.LogDebug("Exiting main()");
#endif /* !NDEBUG */
    }
  return (exit_code);
}
