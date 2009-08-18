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

#include "db/mysql/update.h"

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
void MySQLUpdate::CheckArg()
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
void MySQLUpdate::GenerateQueryBeginning()
{
  this->query = "UPDATE `";
  this->query.append(this->table);
  this->query.append("` SET ");
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief MySQLUpdate copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] myupdate Object to copy data from.
 */
MySQLUpdate::MySQLUpdate(const MySQLUpdate& myupdate)
  : HaveArgs(myupdate),
    Query(myupdate),
    HavePredicate(myupdate),
    HaveFields(myupdate),
    Update(myupdate),
    MySQLHaveArgs(myupdate),
    MySQLHavePredicate(myupdate) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] myupdate Object to copy data from.
 *
 *  \return *this
 */
MySQLUpdate& MySQLUpdate::operator=(const MySQLUpdate& myupdate)
{
  this->MySQLHaveArgs::operator=(myupdate);
  this->MySQLHavePredicate::operator=(myupdate);
  this->Update::operator=(myupdate);
  return (*this);
}

/**
 *  \brief Get the number of argument the query accepts.
 *
 *  This method is used internally by MySQLHaveArgs to allocate memory for
 *  future arguments that will be set by user.
 *
 *  \return Number of arguments that can be set in the query.
 */
unsigned int MySQLUpdate::GetArgCount() throw ()
{
  return (this->fields.size() + this->placeholders);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLUpdate constructor.
 *
 *  Build the UPDATE query using the MySQL connection object on which the query
 *  will operate.
 *
 *  \param[in] myconn MySQL connection object.
 */
MySQLUpdate::MySQLUpdate(MYSQL* myconn) : MySQLHaveArgs(myconn) {}

/**
 *  \brief MySQLUpdate destructor.
 *
 *  Release acquired ressources.
 */
MySQLUpdate::~MySQLUpdate() {}

/**
 *  \brief Execute the update.
 *
 *  Effectively execute the UPDATE query on the MySQL server.
 */
void MySQLUpdate::Execute()
{
  // XXX : does not support multiple execution
  if (!this->stmt)
    this->ProcessPredicate(this->query);
  this->MySQLHaveArgs::Execute();
  return ;
}

/**
 *  \brief Return the number of row affected by the last query.
 *
 *  Ask the MySQL server how many rows were affected by the last query executed
 *  by this object.
 *
 *  \return Number of rows affected by the last query.
 */
unsigned int MySQLUpdate::GetUpdateCount()
{
  unsigned int ret;

  if (this->stmt)
    ret = mysql_stmt_affected_rows(this->stmt);
  else
    ret = mysql_affected_rows(this->mysql);
  return (ret);
}

/**
 *  \brief Prepare the query for execution.
 *
 *  Prepare the UPDATE statement on the MySQL server for later execution.
 */
void MySQLUpdate::Prepare()
{
  // "UPDATE table SET "
  this->GenerateQueryBeginning();

  // Append "field1=?, field2=?, ..., fieldN=?"
  for (std::list<std::string>::iterator it = this->fields.begin();
       it != this->fields.end();
       it++)
    {
      this->query.append(*it);
      this->query.append("=?, ");
    }
  this->query.resize(this->query.size() - 2);

  // Append predicate (if any)
  this->PreparePredicate(this->query);

  // Prepare the query against the server
  this->MySQLHaveArgs::Prepare();

  return ;
}

/**
 *  Set the next argument as a bool.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLUpdate::SetArg(bool arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a double.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLUpdate::SetArg(double arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as an int.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLUpdate::SetArg(int arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a short.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLUpdate::SetArg(short arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a string.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLUpdate::SetArg(const std::string& arg)
{
  if (!this->stmt)
    this->CheckArg();
  this->MySQLHaveArgs::SetArg(arg);
  this->query.append(", ");
  return ;
}

/**
 *  Set the next argument as a time_t.
 *
 *  \param[in] arg Next argument value.
 */
void MySQLUpdate::SetArg(time_t arg)
{
  this->MySQLUpdate::SetArg((int)arg);
  return ;
}
