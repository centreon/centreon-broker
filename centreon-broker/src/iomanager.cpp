/*
** iomanager.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/05/09 Matthieu Kermagoret
*/

#include <cstdlib>
#include <sys/select.h>
#include "iomanager.h"

using namespace CentreonBroker;

IOManager* IOManager::instance = NULL;
Mutex IOManager::mutex;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                ERRORMANAGER                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  ErrorManager constructor.
 */
ErrorManager::ErrorManager()
{
}

/**
 *  ErrorManager copy constructor.
 */
ErrorManager::ErrorManager(const ErrorManager& em)
{
  (void)em;
}

/**
 *  ErrorManager destructor.
 */
ErrorManager::~ErrorManager()
{
}

/**
 *  ErrorManager operator= overload.
 */
ErrorManager& ErrorManager::operator=(const ErrorManager& em)
{
  (void)em;
  return (*this);
}

/******************************************************************************
*                                                                             *
*                                                                             *
*                                READMANAGER                                  *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  ReadManager constructor.
 */
ReadManager::ReadManager()
{
}

/**
 *  ReadManager copy constructor.
 */
ReadManager::ReadManager(const ReadManager& rm)
{
  (void)rm;
}

/**
 *  ErrorManager destructor.
 */
ReadManager::~ReadManager()
{
}

/**
 *  ReadManager operator= overload.
 */
ReadManager& ReadManager::operator=(const ReadManager& rm)
{
  (void)rm;
  return (*this);
}

/******************************************************************************
*                                                                             *
*                                                                             *
*                                WRITEMANAGER                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  WriteManager constructor.
 */
WriteManager::WriteManager()
{
}

/**
 *  WriteManager copy constructor.
 */
WriteManager::WriteManager(const WriteManager& wm)
{
  (void)wm;
}

/**
 *  WriteManager destructor.
 */
WriteManager::~WriteManager()
{
}

/**
 *  WriteManager operator= overload.
 */
WriteManager& WriteManager::operator=(const WriteManager& wm)
{
  (void)wm;
  return (*this);
}

/******************************************************************************
*                                                                             *
*                                                                             *
*                                  IOMANAGER                                  *
*                                                                             *
*                                                                             *
******************************************************************************/

/**
 *  This static function will be set to run upon termination if the IOManager
 *  has been instantiated.
 */
static void delete_iomanager()
{
  delete (IOManager::GetInstance());
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  IOManager constructor.
 */
IOManager::IOManager()
{
  this->map.clear();
}

/**
 *  IOManager copy constructor.
 */
IOManager::IOManager(const IOManager& om)
{
  (void)om;
}

/**
 *  IOManager operator= overload.
 */
IOManager& IOManager::operator=(const IOManager& om)
{
  (void)om;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  IOManager destructor.
 */
IOManager::~IOManager()
{
}

/**
 *  This method build or retrieve the instance of IOManager.
 */
IOManager* IOManager::GetInstance()
{
  if (!IOManager::instance)
    {
      IOManager::mutex.Lock();
      if (!IOManager::instance)
	{
	  IOManager::instance = new IOManager();
	  atexit(delete_iomanager);
	}
    }
  return (IOManager::instance);
}

/**
 *  Set handlers of a FD.
 */
void IOManager::SetFD(int fd,
                      ErrorManager* em,
                      ReadManager* rm,
                      WriteManager* wm)
{
  this->map_mutex.Lock();
  if (!em && !rm && !wm)
    {
      this->map.erase(fd);
    }
  else
    {
      FDManager& fdm = this->map[fd];
      fdm.em = em;
      fdm.rm = rm;
      fdm.wm = wm;
    }
  this->map_mutex.Unlock();
  return ;
}

/**
 *  Wait for events on FDs and call appropriate callbacks.
 */
void IOManager::WaitForEvents() throw (Exception)
{
  fd_set errorfds;
  fd_set readfds;
  fd_set writefds;
  int maxfd;
  std::map<int, FDManager>::iterator it;

  FD_ZERO(&errorfds);
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  maxfd = 0;
  for (it = this->map.begin(); it != this->map.end(); it++)
    {
      if ((*it).first > maxfd)
	maxfd = (*it).first;
      if ((*it).second.em)
	FD_SET((*it).first, &errorfds);
      if ((*it).second.rm)
	FD_SET((*it).first, &readfds);
      if ((*it).second.wm && (*it).second.wm->IsWaitingToWrite())
	FD_SET((*it).first, &writefds);
    }
  if (select(maxfd + 1, &readfds, &writefds, &errorfds, NULL) == -1)
    throw (Exception("Error on IO multiplexing.\n"));
  for (it = this->map.begin(); it != this->map.end(); it++)
    {
      if ((*it).second.em && FD_ISSET((*it).first, &errorfds))
	{
	  (*it).second.em->OnError((*it).first);
	}
      else
	{
	  if ((*it).second.rm && FD_ISSET((*it).first, &readfds))
	    (*it).second.rm->OnRead((*it).first);
	  if ((*it).second.wm && FD_ISSET((*it).first, &writefds))
	    (*it).second.wm->OnWrite((*it).first);
	}
    }
  return ;
}
