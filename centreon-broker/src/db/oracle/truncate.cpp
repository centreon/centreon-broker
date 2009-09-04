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

#include "db/oracle/truncate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief OracleTruncate copy constructor.
 *
 *  Construct the current instance by copying data from the given object.
 *
 *  \param[in] otruncate Object to copy data from.
 */
OracleTruncate::OracleTruncate(const OracleTruncate& otruncate)
  : HaveTable(otruncate),
    Query(otruncate),
    Truncate(otruncate),
    OracleQuery(otruncate) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] otruncate Object to copy data from.
 *
 *  \return *this
 */
OracleTruncate& OracleTruncate::operator=(const OracleTruncate& otruncate)
{
  this->Truncate::operator=(otruncate);
  this->OracleQuery::operator=(otruncate);
  return (*this);
}

/**
 *  \brief Build the query string.
 *
 *  Build the literal TRUNCATE query.
 */
void OracleTruncate::BuildQuery()
{
  this->query = "TRUNCATE TABLE \"";
  this->query.append(this->table);
  this->query.append("\"");
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief OracleTruncate constructor.
 *
 *  Initialize the Oracle TRUNCATE query.
 *
 *  \param[in] oconn Oracle connection object.
 */
OracleTruncate::OracleTruncate(OCI_Connection* oconn) : OracleQuery(oconn) {}

/**
 *  \brief OracleTruncate destructor.
 *
 *  Release all acquired ressources.
 */
OracleTruncate::~OracleTruncate() {}

/**
 *  \brief Execute the query.
 *
 *  Execute the TRUNCATE query on the specified table.
 */
void OracleTruncate::Execute()
{
  // Build the query string if necessary
  if (!this->stmt)
    this->BuildQuery();

  // Really execute the query
  this->OracleQuery::Execute();

  return ;
}

/**
 *  \brief Prepare the query.
 *
 *  Prepare the TRUNCATE query on the Oracle server.
 */
void OracleTruncate::Prepare()
{
  // Build the query string
  this->BuildQuery();

  // Prepare the statement
  this->OracleQuery::Prepare();
  return ;
}
