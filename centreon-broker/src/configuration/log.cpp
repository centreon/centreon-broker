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

#include <syslog.h>            // for LOG_USER
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
Log::Log() : facility(LOG_USER), flags(0), type(Log::UNKNOWN) {}

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
  this->facility = l.facility;
  this->file     = l.file;
  this->flags    = l.flags;
  this->name     = l.name;
  this->type     = l.type;
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
  // XXX : checks should be more extensive.
  return ((this->facility == l.facility)
          && (this->file  == l.file)
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

/**
 *  Overload of the inferior operator.
 *
 *  \param[in] l Object to compare to.
 *
 *  \return true if this object is stricly less than l.
 */
bool Log::operator<(const Log& l) const
{
  bool retval;

  if (this->type != l.type)
    retval = (this->type < l.type);
  else if (this->facility != l.facility)
    retval = (this->facility < l.facility);
  else if (this->flags != l.flags)
    retval = (this->flags < l.flags);
  else if (this->file != l.file)
    retval = (this->file < l.file);
  else
    retval = (this->name < l.name);
  return (retval);
}
