/*
** cb2db.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/13/09 Matthieu Kermagoret
** Last update 05/14/09 Matthieu Kermagoret
*/

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <vector>
#include "mysqloutput.h"
#include "networkinput.h"

using namespace CentreonBroker;

// XXX : code is not really clean, it should have error checking.
int main()
{
  int fd;
  struct sockaddr_in sin;
  MySQLOutput* mo;
  std::vector<NetworkInput*> vni;
  NetworkInput* ni;

  mo = new MySQLOutput();
  mo->Init("localhost", "root", "123456789", "ndo");
  fd = socket(PF_INET, SOCK_STREAM, 0);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(5667);
  sin.sin_addr.s_addr = INADDR_ANY;
  bind(fd, (struct sockaddr*)&sin, sizeof(sin));
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  listen(fd, 0);
  while (1)
    {
      fd_set error_fds;
      fd_set read_fds;

      FD_ZERO(&error_fds);
      FD_SET(STDIN_FILENO, &error_fds);
      FD_SET(fd, &error_fds);
      FD_ZERO(&read_fds);
      FD_SET(STDIN_FILENO, &read_fds);
      FD_SET(fd, &read_fds);
      if (-1 == select(fd + 1, &read_fds, NULL, &error_fds, NULL))
	{
	  if (EINTR != errno)
	    break ;
	  else
	    continue ;
	}
      if (FD_ISSET(STDIN_FILENO, &error_fds)
	  || FD_ISSET(fd, &error_fds)
	  || FD_ISSET(STDIN_FILENO, &read_fds))
	break ;
      ni = new NetworkInput();
      vni.push_back(ni);
      ni->SetFD(accept(fd, NULL, NULL));
      listen(fd, 0);
    }
  for (std::vector<NetworkInput*>::iterator it = vni.begin();
       it != vni.end();
       it++)
    delete (*it);
  mo->Destroy();
  delete mo;
  return (0);
}
