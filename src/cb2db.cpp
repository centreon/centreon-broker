/*
** cb2db.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/13/09 Matthieu Kermagoret
** Last update 06/12/09 Matthieu Kermagoret
*/

#include <boost/asio.hpp>
#include <csignal>
#include <cstdlib>
#include <mysql.h>
#include "db_output.h"
#include "logging.h"
#include "mapping.h"
#include "network_acceptor.h"

using namespace CentreonBroker;

static boost::asio::io_service* gl_ios;
static DBOutput* gl_dbo;
static NetworkAcceptor* gl_na;
static volatile bool gl_shall_exit = false;

static void term_handler(int signum)
{
  (void)signum;
  gl_shall_exit = true;
  gl_ios->stop();
  return ;
}

int main(int argc, char *argv[])
{
  if (argc != 5)
    {
      std::string usage;

      usage = "USAGE: ";
      usage += argv[0];
      usage += " <server> <user> <password> <db>";
      logging.AddInfo(usage.c_str());
      return (1);
    }
  else
    {
      logging.AddDebug("Initializing MySQL library...");
      mysql_library_init(0, NULL, NULL);
      logging.AddDebug("Initializing I/O engine...");
      gl_ios = new boost::asio::io_service;
      InitMappings();
      logging.AddDebug("Connecting to MySQL server...");
      logging.Indent();
      gl_dbo = new DBOutput(DB::Connection::MYSQL);
      gl_dbo->Init(argv[1], argv[2], argv[3], argv[4]);
      logging.Deindent();
      logging.AddDebug("Listening for new connections...");
      logging.Indent();
      gl_na = new NetworkAcceptor(*gl_ios);
      gl_na->Accept(5667);
      logging.Deindent();
      signal(SIGINT, term_handler);
      try
	{
	  while (!gl_shall_exit)
	    {
	      gl_ios->run();
	      gl_ios->reset();
	    }
	}
      catch (boost::system::system_error& se)
	{
	}
      if (gl_na)
	{
	  delete gl_na;
	  gl_na = NULL;
	}
      if (gl_dbo)
	{
	  gl_dbo->Destroy();
	  delete gl_dbo;
	  gl_dbo = NULL;
	}
      if (gl_ios)
	{
	  logging.AddDebug("Shutting down I/O service...");
	  delete gl_ios;
	  gl_ios = NULL;
	}
      logging.AddDebug("Exiting main()");
    }
  return (0);
}
