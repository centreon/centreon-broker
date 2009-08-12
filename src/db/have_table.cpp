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

#include "db/have_table.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief HaveTable default constructor.
 *
 *  Initialize members to their default values.
 */
HaveTable::HaveTable() {}

/**
 *  \brief HaveTable copy constructor.
 *
 *  Copy the table name of the given object to the current instance. Because
 *  users aren't supposed to directly copy-construct a HaveTable object, this
 *  constructor is declared protected.
 *
 *  \param[in] ht Object to copy the table name from.
 */
HaveTable::HaveTable(const HaveTable& ht)
{
  this->table = ht.table;
}

/**
 *  \brief HaveTable destructor.
 *
 *  Release all acquired ressources.
 */
HaveTable::~HaveTable() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the table name of the given object to the current instance. Because
 *  users aren't supposed to directly copy a HaveTable object, this method is
 *  declared protected.
 *
 *  \param[in] ht Object to copy the table name from.
 *
 *  \return *this
 */
HaveTable& HaveTable::operator=(const HaveTable& ht)
{
  this->table = ht.table;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Get the table name.
 *
 *  Returns the name of the table on which the query operates.
 *
 *  \return The name of the table.
 */
const std::string& HaveTable::GetTable() throw ()
{
  return (this->table);
}

/**
 *  \brief Set the table name.
 *
 *  Set the name of the table on which the query operates.
 *
 *  \param[in] The name of the table.
 */
void HaveTable::SetTable(const std::string& tab)
{
  this->table = tab;
  return ;
}
