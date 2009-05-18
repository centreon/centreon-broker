/*
** cb2db.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/13/09 Matthieu Kermagoret
** Last update 05/18/09 Matthieu Kermagoret
*/

#include <unistd.h>
#include "mysqloutput.h"
#include "network_acceptor.h"

using namespace CentreonBroker;

// XXX : code is not really clean, it should have error checking.
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
      char c;
      MySQLOutput* mo;
      NetworkAcceptor* na;

      mo = new MySQLOutput();
      std::clog << "Connecting to MySQL server : " << argv[2]
                << '@' << argv[1] << std::endl;
      mo->Init(argv[1], argv[2], argv[3], argv[4]);
      na = new NetworkAcceptor();
      na->SetPort(5667);
      na->Listen();
      read(STDIN_FILENO, &c, 1);
      std::clog << "Closing listening socket." << std::endl;
      delete na;
      std::clog << "Synchronizing with MySQL server." << std::endl;
      mo->Destroy();
      delete mo;
      std::clog << "Exiting." << std::endl;
    }
  return (0);
}
