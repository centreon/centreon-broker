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
#include "db/oracle/query.h"
#include "logging.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief OracleQuery constructor.
 *
 *  Initialize the new instance with the Oracle connection object from which it
 *  depends.
 *
 *  \param[in] oconn Oracle connection on which the query will be executed.
 */
OracleQuery::OracleQuery(OCI_Connection* oconn)
  : oconn_(oconn), plain_stmt(NULL), stmt(NULL) {}

/**
 *  \brief OracleQuery copy constructor.
 *
 *  Build the Oracle query by copying data from the given query.
 *
 *  \param[in] oquery Object to copy data from.
 */
OracleQuery::OracleQuery(const OracleQuery& oquery) : Query(oquery)
{
  // XXX : copy data
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given query object to the current instance.
 *
 *  \param[in] oquery Object to copy data from.
 *
 *  \return *this
 */
OracleQuery& OracleQuery::operator=(const OracleQuery& oquery)
{
  this->Query::operator=(oquery);
  // XXX : copy data
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief OracleQuery destructor.
 *
 *  Close the statement if the Prepare() method has been called.
 */
OracleQuery::~OracleQuery()
{
  if (this->plain_stmt)
    OCI_StatementFree(this->plain_stmt);
  if (this->stmt)
    OCI_StatementFree(this->stmt);
}

/**
 *  \brief Execute the query.
 *
 *  Execute the underlying query on the Oracle server.
 */
void OracleQuery::Execute()
{
  OCI_Statement* statement;

  if (this->plain_stmt)
    {
      OCI_StatementFree(this->plain_stmt);
      this->plain_stmt = NULL;
    }
  if (this->stmt)
    {
#ifndef NDEBUG
      logging.LogDebug("Executing Oracle standard query ...");
      logging.LogDebug(this->query.c_str());
#endif /* !NDEBUG */
      this->plain_stmt = OCI_StatementCreate(this->oconn_);
      if (!this->plain_stmt)
        {
          OCI_Error* err;

          err = OCI_GetLastError();
          throw (DBException(OCI_ErrorGetOCICode(err),
                             DBException::QUERY_EXECUTION,
                             OCI_ErrorGetString(err)));
	}
      statement = this->plain_stmt;
    }
  else
    {
#ifndef NDEBUG
      logging.LogDebug("Executing Oracle prepared statement ...");
#endif /* !NDEBUG */
      statement = this->stmt;
    }

  if (!OCI_Execute(statement))
    {
      OCI_Error* err;

      err = OCI_GetLastError();
      DBException dbe(OCI_ErrorGetOCICode(err),
                      DBException::QUERY_EXECUTION,
                      OCI_ErrorGetString(err));
      if (this->plain_stmt)
        {
          OCI_StatementFree(this->plain_stmt);
          this->plain_stmt = NULL;
        }
      throw (dbe);
    }
  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  Prepare the query on the Oracle server for later execution.
 */
void OracleQuery::Prepare()
{
#ifndef NDEBUG
  logging.LogDebug("Preparing Oracle statement...");
  logging.LogDebug(this->query.c_str());
#endif /* !NDEBUG */
  this->stmt = OCI_CreateStatement(this->oconn_);
  if (!this->stmt)
    {
      OCI_Error* err;

      err = OCI_GetLastError();
      throw (DBException(OCI_ErrorGetOCICode(err),
                         DBException::QUERY_PREPARATION,
                         OCI_ErrorGetString(err)));
    }
  if (!OCI_Prepare(this->stmt, this->query.c_str()))
    {
      OCI_Error* err;

      err = OCI_GetLastError();
      throw (DBException(OCI_ErrorGetOCICode(err),
                         DBException::QUERY_PREPARATION,
                         OCI_ErrorGetString(err)));
    }
  return ;
}
