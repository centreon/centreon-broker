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

#include "db/oracle/delete.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Generate the first part of the query string.
 *
 *  Expand the query object to a literal query string comprehensible by Oracle.
 */
void OracleDelete::GenerateQueryBeginning()
{
  this->query = "DELETE FROM \"";
  this->query.append(this->table);
  this->query.append("\"");
  return ;
}

/**
 *  \brief Returns the number of argument this query accepts.
 *
 *  This overriden method is used by OracleHaveArgs to allocate its argument
 *  buffer.
 *
 *  \return Number of arguments to be set in this query.
 */
unsigned int OracleDelete::GetArgCount() throw ()
{
  return (this->placeholders);
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief OracleDelete copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] odelete Object to copy data from.
 */
OracleDelete::OracleDelete(const OracleDelete& odelete)
  : HaveArgs(odelete),
    HavePredicate(odelete),
    Query(odelete),
    Delete(odelete),
    OracleHaveArgs(odelete),
    OracleHavePredicate(odelete) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] odelete Object to copy data from.
 *
 *  \return *this
 */
OracleDelete& OracleDelete::operator=(const OracleDelete& odelete)
{
  this->Delete::operator=(odelete);
  this->OracleHaveArgs::operator=(odelete);
  this->OracleHavePredicate::operator=(odelete);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief OracleDelete constructor. Needs the Oracle connection object.
 *
 *  Build a new MySQL DELETE query.
 *
 *  \param[in] oconn The Oracle connection object on which the query will be
 *                   executed.
 */
OracleDelete::OracleDelete(OCI_Connection* oconn) : OracleHaveArgs(oconn) {}

/**
 *  \brief OracleDelete destructor.
 *
 *  Release all acquired ressources.
 */
OracleDelete::~OracleDelete() {}

/**
 *  \brief Execute the DELETE query.
 *
 *  Execute the DELETE query on the Oracle server as it has been configured.
 */
void OracleDelete::Execute()
{
  // If the query has not been prepared, generate the query string
  if (!this->stmt)
    {
      // Generate the first part of the query.
      this->GenerateQueryBeginning();

      // Generate the predicate string (if any).
      this->OracleHavePredicate::ProcessPredicate(this->query);
    }

  // Execute the query (prepared or not).
  this->OracleHaveArgs::Execute();

  return ;
}

/**
 *  \brief Prepare the DELETE query.
 *
 *  Prepare the DELETE query on the Oracle server.
 */
void OracleDelete::Prepare()
{
  // Generate the query string.
  this->GenerateQueryBeginning();

  // Append the predicate (if any).
  this->OracleHavePredicate::PreparePredicate(this->query);

  // Prepare the query against the DB server.
  this->OracleHaveArgs::Prepare();

  return ;
}
