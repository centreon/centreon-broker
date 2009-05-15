/*
** event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/14/09 Matthieu Kermagoret
*/

#include "event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Event default constructor.
 */
Event::Event()
{
  this->readers_ = 0;
}

/**
 *  Event copy constructor.
 */
Event::Event(const Event& event)
{
  operator=(event);
}

/**
 *  Event destructor.
 */
Event::~Event()
{
}

/**
 *  Event operator= overload. Only copy the event instance.
 */
Event& Event::operator=(const Event& event)
{
  (void)event;
  this->nagios_instance_ = event.nagios_instance_;
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
 *  Returns the instance on which the event occured.
 */
const std::string& Event::GetNagiosInstance() const throw ()
{
  return (this->nagios_instance_);
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

/**
 *  Sets the Nagios instance on which the event occured.
 */
void Event::SetNagiosInstance(const std::string& inst)
{
  this->nagios_instance_ = inst;
  return ;
}
