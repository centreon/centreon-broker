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

#include <assert.h>
#include <stdlib.h>                  // for abort
#include "input/feeder.h"
#include "interface/source.h"
#include "multiplexing/publisher.h"

using namespace Input;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Feeder copy constructor.
 *
 *  Feeder is not copyable. Any attempt to use the copy constructor will result
 *  in a call to abort().
 *
 *  \param[in] feeder Unused.
 */
Feeder::Feeder(const Feeder& feeder)
{
  (void)feeder;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Feeder is not copyable. Any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] feeder Unused.
 *
 *  \return *this
 */
Feeder& Feeder::operator=(const Feeder& feeder)
{
  (void)feeder;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Feeder default constructor.
 */
Feeder::Feeder() {}

/**
 *  Feeder destructor.
 */
Feeder::~Feeder()
{
  try
    {
      if (this->source_.get())
        {
          // XXX : is this thread safe ?
          this->source_->Close();
          // Necessary to join the thread, because the destructor might end
          // before the thread terminates, leading to undefined behavior.
          this->thread_.Join();
        }
    }
  catch (...) {}
}

/**
 *  \brief Overload of the parenthesis operator.
 *
 *  This method is used as the entry point of the thread that will get events
 *  from the source.
 *  \par Safety No throw guarantee.
 */
void Feeder::operator()()
{
  try
    {
      std::auto_ptr<Events::Event> event;

      event.reset(this->source_->Event());
      while (event.get())
        {
          Multiplexing::Publisher::Instance().Publish(event.get());
          event.release();
          event.reset(this->source_->Event());
        }
      // XXX : warn Manager
    }
  catch (...) {}
  return ;
}

/**
 *  \brief Initialize the Feeder.
 *
 *  Launch the processing thread. The thread will get events from the source
 *  specified as a parameter and give them to the event publisher. Upon
 *  successful return from this method, the source object's lifetime will be
 *  handled by the Feeder.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] source Event source object.
 */
void Feeder::Init(Interface::Source* source)
{
  this->source_.reset(source);
  try
    {
      this->thread_.Run(this);
    }
  catch (...)
    {
      this->source_.release();
      throw ;
    }
  return ;
}
