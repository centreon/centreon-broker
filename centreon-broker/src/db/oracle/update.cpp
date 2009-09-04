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

#include "db/oracle/update.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Check if it's the first time an argument is set.
 *
 *  If it is the first time, the query beginning will be generated. This method
 *  is called only when using plain text query.
 */
void OracleUpdate::CheckArg()
{
  if (this->query.empty())
    {
      this->GenerateQueryBeginning();
      this->field_ = this->fields.begin();
    }
  this->query.append(*(this->field_));
  this->field_++;
  this->query.append("=");
  return ;
}

/**
 *  \brief Generate the beginning of the UPDATE query.
 *
 *  Generate the part common to prepared statements and normal queries :
 *  "UPDATE table SET ".
 */
void OracleUpdate::GenerateQueryBeginning()
{
  this->query = "UPDATE \"";
  this->query.append(this->table);
  this->query.append("\" SET ");
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief OracleUpdate copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] oupdate Object to copy data from.
 */
OracleUpdate::OracleUpdate(const OracleUpdate& oupdate)
  : HaveArgs(oupdate),
    Query(oupdate),
    HavePredicate(oupdate),
    HaveFields(oupdate),
    Update(oupdate),
    OracleHaveArgs(oupdate),
    OracleHavePredicate(oupdate) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] oupdate Object to copy data from.
 *
 *  \return *this
 */
OracleUpdate& OracleUpdate::operator=(const OracleUpdate& oupdate)
{
  this->OracleHaveArgs::operator=(oupdate);
  this->OracleHavePredicate::operator=(oupdate);
  this->Update::operator=(oupdate);
  return (*this);
}

/**
 *  \brief Get the number of argument the query accepts.
 *
 *  This method is used internally by OracleHaveArgs to allocate memory for
 *  future arguments that will be set by user.
 *
 *  \return Number of arguments that can be set in the query.
 */
unsigned int OracleUpdate::GetArgCount() throw ()
{
  // XXX : not really exception safe
  return (this->fields.size() + this->placeholders);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief OracleUpdate constructor.
 *
 *  Build the UPDATE query using the Oracle connection object on which the
 *  query will operate.
 *
 *  \param[in] oconn Oracle connection object.
 */
OracleUpdate::OracleUpdate(OCI_Connection* oconn) : OracleHaveArgs(oconn) {}

/**
 *  \brief OracleUpdate destructor.
 *
 *  Release acquired ressources.
 */
OracleUpdate::~OracleUpdate() {}

/**
 *  \brief Execute the update.
 *
 *  Effectively execute the UPDATE query on the Oracle server.
 */
void OracleUpdate::Execute()
{
  // XXX : does not support multiple execution
  if (!this->stmt)
    this->ProcessPredicate(this->query);
  this->OracleHaveArgs::Execute();
  return ;
}

/**
 *  \brief Return the number of row affected by the last query.
 *
 *  Ask the Oracle server how many rows were affected by the last query
 *  executed by this object.
 *
 *  \return Number of rows affected by the last query.
 */
unsigned int OracleUpdate::GetUpdateCount()
{
  // XXX
}

/**
 *  \brief Prepare the query for execution.
 *
 *  Prepare the UPDATE statement on the Oracle server for later execution.
 */
void OracleUpdate::Prepare()
{
  // "UPDATE table SET "
  this->GenerateQueryBeginning();

  // Append "field1=?, field2=?, ..., fieldN=?"
  unsigned int i = 0;
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       it++)
    {
      std::ostringstream ss;

      ss << *it << "=:" << ++i << ", ";
      this->query.append(ss.str());
    }
  this->query.resize(this->query.size() - 2);

  // Append predicate (if any)
  this->PreparePredicate(this->query);

  // Prepare the query against the server
  this->OracleHaveArgs::Prepare();

  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next argument value.
 */
void OracleUpdate::SetArg(bool arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next argument value.
 */
void OracleUpdate::SetArg(double arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next argument value.
 */
void OracleUpdate::SetArg(int arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next argument value.
 */
void OracleUpdate::SetArg(short arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next argument value.
 */
void OracleUpdate::SetArg(const std::string& arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->OracleHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next argument value.
 */
void OracleUpdate::SetArg(time_t arg)
{
  this->OracleUpdate::SetArg((int)arg);
  return ;
}
