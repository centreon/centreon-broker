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

#include "configuration/log.h"

using namespace Configuration;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Log default constructor.
 *
 *  Initialize members to default values.
 */
Log::Log() : flags(0), type(Log::UNKNOWN) {}

/**
 *  \brief Log copy constructor.
 *
 *  Copy all parameters of the given object to the current instance.
 *
 *  \param[in] l Object to copy data from.
 */
Log::Log(const Log& l)
{
  this->operator=(l);
}

/**
 *  Log destructor.
 */
Log::~Log() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all parameters of the given object to the current instance.
 *
 *  \param[in] l Object to copy data from.
 *
 *  \return *this
 */
Log& Log::operator=(const Log& l)
{
  this->file  = l.file;
  this->flags = l.flags;
  this->name  = l.name;
  this->type  = l.type;
  return (*this);
}

/**
 *  \brief Overload of the equal to operator.
 *
 *  Check if the current instance and the given object are equal. To be
 *  declared equals, every parameter should be equal.
 *
 *  \return True if the two objects are equal, false otherwise.
 */
bool Log::operator==(const Log& l) const
{
  return ((this->file     == l.file)
          && (this->flags == l.flags)
          && (this->name  == l.name)
          && (this->type  == l.type));
}

/**
 *  Overload of the not equal to operator.
 *
 *  \return The complement of the operator== return value.
 *
 *  \see operator==
 */
bool Log::operator!=(const Log& l) const
{
  return (!(*this == l));
}
