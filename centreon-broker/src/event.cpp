/*
** event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/12/09 Matthieu Kermagoret
*/

#include "event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Event constructor.
 */
Event::Event()
{
  this->readers_ = 0;
}

/**
 *  Event copy constructor (does nothing more than standard constructor).
 */
Event::Event(const Event& event)
{
  (void)event;
  this->readers_ = 0;
}

/**
 *  Event destructor.
 */
Event::~Event()
{
}

/**
 *  Event operator= overload (does nothing).
 */
Event& Event::operator=(const Event& event)
{
  (void)event;
  return (*this);
}

/**
 *  Specify that somebody is reading the Event. It shall not be destructed
 *  until the reader specify that he's done with the event.
 *
 *  For now, AddReader() only counts the number of times it has been called.
 */
void Event::AddReader(EventSubscriber* es)
{
  (void)es;
  this->mutex_.Lock();
  this->readers_++;
  this->mutex_.Unlock();
  return ;
}

/**
 *  Remove an event reader. The current implementation only counts the number
 *  of calls made to AddReader and RemoveReader and if the numbers equal, the
 *  object self-destruct.
 */
void Event::RemoveReader(EventSubscriber* es)
{
  (void)es;
  this->mutex_.Lock();
  if (--this->readers_ <= 0)
    {
      this->mutex_.Unlock();
      delete (this);
    }
  else
    this->mutex_.Unlock();
  return ;
}
