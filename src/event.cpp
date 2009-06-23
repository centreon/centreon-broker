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
  this->readers_ = 0;
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
  boost::unique_lock<boost::mutex> lock(this->mutex_);

  (void)es;
  this->readers_++;
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
  bool destroy;

  (void)es;
  this->mutex_.lock();
  if (--this->readers_ <= 0)
    destroy = true;
  else
    destroy = false;
  this->mutex_.unlock();
  if (destroy)
    delete (this);
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
