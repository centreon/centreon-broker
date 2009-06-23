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

#include <cassert>
#include "db/truncate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Truncate copy constructor.
 */
Truncate::Truncate(const Truncate& truncate) throw () : Query()
{
  (void)truncate;
  assert(false);
}

/**
 *  Truncate operator= overload.
 */
Truncate& Truncate::operator=(const Truncate& truncate) throw ()
{
  (void)truncate;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Truncate default constructor.
 */
Truncate::Truncate() throw ()
{
}

/**
 *  Truncate destructor.
 */
Truncate::~Truncate()
{
}

/**
 *  Returns the table name.
 */
const std::string& Truncate::GetTable() const throw ()
{
  return (this->table_);
}

/**
 *  Set the table name.
 */
void Truncate::SetTable(const std::string& table)
{
  this->table_ = table;
  return ;
}
