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

#include <string>
#include "events/instance.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of the given object to the current
 *  instance.
 */
void Instance::InternalCopy(const Instance& inst)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = inst.strings_[i];
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Instance default constructor.
 */
Instance::Instance()
{
}

/**
 *  Instance copy constructor.
 */
Instance::Instance(const Instance& inst) : Event(inst)
{
  this->InternalCopy(inst);
}

/**
 *  Instance destructor.
 */
Instance::~Instance()
{
}

/**
 *  Instance operator= overload.
 */
Instance& Instance::operator=(const Instance& inst)
{
  this->Event::operator=(inst);
  this->InternalCopy(inst);
  return (*this);
}

/**
 *  Returns the type of this Event.
 */
int Instance::GetType() const throw ()
{
  // XXX : correct with NDO_API_SOMETHING
  return (3);
}
