/*
** cb2db.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/13/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
*/

#include <boost/asio.hpp>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <mysql.h>
#include "mapping.h"
#include "db_output.h"
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
      std::cerr << "USAGE: " << argv[0] << " <server> <user> <password> <db>"
                << std::endl;
      return (1);
    }
  else
    {
      mysql_library_init(0, NULL, NULL);
      std::clog << "Initializing I/O engine...";
      gl_ios = new boost::asio::io_service;
      std::clog << "  Done" << std::endl;
      std::clog << "Initializing MySQL engine...";
      gl_dbo = new DBOutput(DB::Connection::MYSQL);
      std::clog << "  Done" << std::endl;
      std::clog << "Connecting to MySQL server : " << argv[2]
                << '@' << argv[1] << "...";
      gl_dbo->Init(argv[1], argv[2], argv[3], argv[4]);
      std::clog << "  Done" << std::endl;
      std::clog << "Listening on port 5667...";
      gl_na = new NetworkAcceptor(*gl_ios);
      gl_na->Accept(5667);
      std::clog << "  Done" << std::endl;
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
	  std::clog << "Closing listening socket...";
	  delete gl_na;
	  gl_na = NULL;
	  std::clog << "  Done" << std::endl;
	}
      if (gl_dbo)
	{
	  std::clog << "Closing connection to MySQL server...";
	  gl_dbo->Destroy();
	  delete gl_dbo;
	  gl_dbo = NULL;
	  std::clog << "  Done" << std::endl;
	}
      if (gl_ios)
	{
	  std::clog << "Shutting down I/O service...";
	  delete gl_ios;
	  gl_ios = NULL;
	  std::clog << "  Done" << std::endl;
	}
      std::clog << "Exiting now." << std::endl;
    }
  return (0);
}
