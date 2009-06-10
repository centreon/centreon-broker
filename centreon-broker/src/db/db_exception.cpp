/*
** db_exception.cpp for CentreonBroker in ./src/db
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/09/09 Matthieu Kermagoret
** Last update 06/10/09 Matthieu Kermagoret
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
