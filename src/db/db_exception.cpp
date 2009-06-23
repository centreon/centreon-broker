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

#include "db/db_exception.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void DBException::InternalCopy(const DBException& dbe) throw ()
{
  this->reason_ = dbe.reason_;
  return ;
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  DBException copy constructor.
 */
DBException::DBException(const DBException& dbe)
  : CentreonBroker::Exception(dbe)
{
  this->InternalCopy(dbe);
}

/**
 *  Build an exception from an error code, the reason of the exception and the
 *  error message.
 */
DBException::DBException(int val, DBException::Reason reason, const char* msg)
  : CentreonBroker::Exception(val, msg), reason_(reason)
{
}

/**
 *  DBException destructor.
 */
DBException::~DBException() throw ()
{
}

/**
 *  DBException operator= overload.
 */
DBException& DBException::operator=(const DBException& dbe)
{
  this->CentreonBroker::Exception::operator=(dbe);
  this->InternalCopy(dbe);
  return (*this);
}

/**
 *  Return the reason why the exception happened.
 */
DBException::Reason DBException::GetReason() const throw ()
{
  return (this->reason_);
}
