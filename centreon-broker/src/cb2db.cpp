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
#include <iostream>
#include <mysql.h>
#include "client_acceptor.h"
#include "conf/conf.h"
#include "db_output.h"
#include "io/net4.h"
#ifdef USE_TLS
# include "io/tls.h"
#endif /* USE_TLS */
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
  Conf::Conf conf;
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
          mysql_library_init(0, NULL, NULL);

          // Load Object-Relational mappings
          InitMappings();

          // Load configuration file
          conf.Load(argv[1]);

          // Process configuration file
#ifndef NDEBUG
          logging.LogDebug("Processing configuration file...");
          logging.Indent();
#endif /* !NDEBUG */
          {
            const Conf::Input* input;

            input = conf.GetNextInput();
            while (input)
              {
                ClientAcceptor* ca;
		IO::Acceptor* a;

#ifdef USE_TLS
/*		if (input->GetTLS())
		  {
		    IO::TLSAcceptor* tls_acceptor;

		    tls_acceptor = new IO::TLSAcceptor;
		    // XXX : TLS configuration
		    a = tls_acceptor;
		  }
		else
		{*/
#endif /* USE_TLS */
		    IO::Net4Acceptor* net4_acceptor;

		    net4_acceptor = new IO::Net4Acceptor;
		    net4_acceptor->Listen(input->GetPort());
		    a = net4_acceptor;
#ifdef USE_TLS
		    //}
#endif /* USE_TLS */
		ca = new ClientAcceptor;
		ca->Run(a);
                sockets.push_back(ca);
                input = conf.GetNextInput();
              }
          }
          {
            const Conf::Log* l;

            l = conf.GetNextLog();
            while (l)
              {
                if (l->GetType() == "syslog")
                  logging.LogInSyslog(true, l->GetFlags());
                else if (l->GetType() == "file")
                  logging.LogInFile(l->GetPath().c_str(), l->GetFlags());
                l = conf.GetNextLog();
              }
          }
          {
            const Conf::Output* output;

            output = conf.GetNextOutput();
            while (output)
              {
                DBOutput* dbo;

                if (output->GetDbms() == "postgresql")
                  dbo = new DBOutput(DB::Connection::POSTGRESQL);
                else
                  dbo = new DBOutput(DB::Connection::MYSQL);
                dbo->Init(output->GetHost(),
                          output->GetUser(),
                          output->GetPassword(),
                          output->GetDb());
                dbs.push_back(dbo);
                output = conf.GetNextOutput();
              }
          }
#ifndef NDEBUG
          logging.Deindent();
#endif /* !NDEBUG */

          // Catch ^C
          signal(SIGINT, term_handler);

          // Everything loader, ready to go
          logging.LogInfo("Initialization completed, waiting for clients...");
          while (!gl_shall_exit)
	    pause();

          exit_code = 0;
        }
      catch (std::exception& e)
        {
          logging.LogInfo("Process terminated because of this exception :");
          logging.Indent();
          logging.LogInfo(e.what());
          logging.Deindent();
          exit_code = 1;
        }

#ifndef NDEBUG
      logging.LogDebug("Exiting main()");
#endif /* !NDEBUG */
    }
  return (exit_code);
}
