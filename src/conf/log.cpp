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

#include <cstring>
#include "conf/log.h"

using namespace CentreonBroker::Conf;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Log::InternalCopy(const Log& l)
{
  memcpy(this->ints_, l.ints_, sizeof(this->ints_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = l.strings_[i];
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Log default constructor.
 */
Log::Log()
{
  memset(this->ints_, 0, sizeof(this->ints_));
}

/**
 *  Log copy constructor.
 */
Log::Log(const Log& l)
{
  this->InternalCopy(l);
}

/**
 *  Log destructor.
 */
Log::~Log()
{
}

/**
 *  Log operator= overload.
 */
Log& Log::operator=(const Log& l)
{
  this->InternalCopy(l);
  return (*this);
}

/**
 *  Verify if two Log objects are identical.
 */
bool Log::operator==(const Log& l)
{
  return ((this->ints_[FLAGS] == l.ints_[FLAGS])
	  && (this->strings_[PATH] == l.strings_[PATH])
	  && (this->strings_[TYPE] == l.strings_[TYPE]));
}

/**
 *  Get the flags.
 */
int Log::GetFlags() const throw ()
{
  return (this->ints_[FLAGS]);
}

/**
 *  Get the name of the output file.
 */
const std::string& Log::GetPath() const throw ()
{
  return (this->strings_[PATH]);
}

/**
 *  Get the type of the log (syslog or file).
 */
const std::string& Log::GetType() const throw ()
{
  return (this->strings_[TYPE]);
}

/**
 *  Set the flags.
 */
void Log::SetFlags(int flags) throw ()
{
  this->ints_[FLAGS] = flags;
  return ;
}

/**
 *  Set the file name.
 */
void Log::SetPath(const std::string& path)
{
  this->strings_[PATH] = path;
  return ;
}

/**
 *  Set the log type.
 */
void Log::SetType(const std::string& type)
{
  this->strings_[TYPE] = type;
  return ;
}
