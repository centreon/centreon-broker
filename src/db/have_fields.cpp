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

#include <algorithm>
#include "db/have_fields.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief HaveFields default constructor.
 *
 *  Initialize members to their default values.
 */
HaveFields::HaveFields() {}

/**
 *  \brief HaveFields copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] hf Object to copy data from.
 */
HaveFields::HaveFields(const HaveFields& hf) : HaveArgs(hf)
{
  this->fields = hf.fields;
}

/**
 *  \brief HaveFields destructor.
 *
 *  Release previously acquired ressources.
 */
HaveFields::~HaveFields() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] hf Object to copy data from.
 *
 *  \return *this
 */
HaveFields& HaveFields::operator=(const HaveFields& hf)
{
  this->HaveArgs::operator=(hf);
  this->fields = hf.fields;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Add a field on which the query should operate.
 *
 *  Specify that the underlying query should operate on this field.
 *
 *  \param[in] field New field to operate on.
 */
void HaveFields::AddField(const std::string& field)
{
  this->fields.push_back(field);
  return ;
}

/**
 *  \brief Specify that the query shouldn't operate on a field.
 *
 *  Remove the specified field from the list of fields on which the query
 *  operates.
 *
 *  \param[in] field Field that shouldn't be treated within the query.
 */
void HaveFields::RemoveField(const std::string& field)
{
  std::list<std::string>::iterator it;

  // Try to find the field in the list
  it = std::find(this->fields.begin(), this->fields.end(), field);

  // If it does exist, remove it
  if (it != this->fields.end())
    this->fields.erase(it);

  return ;
}
