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

#include "events/check.h"

using namespace Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy internal data of the given object to the current instance.
 *
 *  \param[in] check Object to copy from.
 */
void Check::InternalCopy(const Check& check)
{
  this->command_line = check.command_line;
  this->host_id      = check.host_id;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
Check::Check() : host_id(0) {}

/**
 *  Copy constructor.
 *
 *  \param[in] check Object to copy from.
 */
Check::Check(const Check& check) : Event(check)
{
  this->InternalCopy(check);
}

/**
 *  Destructor.
 */
Check::~Check() {}

/**
 *  Overload of the assignment operator.
 *
 *  \param[in] check Object to copy from.
 *
 *  \return *this
 */
Check& Check::operator=(const Check& check)
{
  this->Event::operator=(check);
  this->InternalCopy(check);
  return (*this);
}
