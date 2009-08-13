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
MySQLUpdate::MySQLUpdate(const MySQLUpdate& myupdate) {}

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
