/*
** networkinput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/11/09 Matthieu Kermagoret
*/

#include <unistd.h>
#include "networkinput.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

NetworkInput::NetworkInput(const NetworkInput& ni) : Thread()
{
  (void)ni;
}

NetworkInput& NetworkInput::operator=(const NetworkInput& ni)
{
  (void)ni;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

NetworkInput::NetworkInput()
{
  this->fd = -1;
}

NetworkInput::~NetworkInput()
{
  if (this->fd >= 0)
    {
      close(this->fd);
      this->Join();
    }
}

int NetworkInput::Core()
{
  // XXX : test stuff
  return (0);
}

void NetworkInput::SetFD(int fd)
{
  this->fd = fd;
  this->Run();
  return ;
}
