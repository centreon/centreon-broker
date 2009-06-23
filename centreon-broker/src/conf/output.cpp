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

#include <cstring>
#include "conf/output.h"

using namespace CentreonBroker::Conf;

/**
 *  Output default constructor.
 */
Output::Output()
{
}

/**
 *  Output copy constructor.
 */
Output::Output(const Output& output)
{
  this->operator=(output);
}

/**
 *  Output destructor.
 */
Output::~Output()
{
}

/**
 *  Output operator= overload.
 */
Output& Output::operator=(const Output& output)
{
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = output.strings_[i];
  return (*this);
}

/**
 *  Output operator== overload.
 */
bool Output::operator==(const Output& output)
{
  bool result;

  result = true;
  for (unsigned int i = 0; i < STRING_NB; i++)
    result = result && (output.strings_[i] == this->strings_[i]);
  return (result);
}

/**
 *  Returns the DB name.
 */
const std::string& Output::GetDb() const throw ()
{
  return (this->strings_[DB]);
}

/**
 *  Returns the host name to use when connecting to DB.
 */
const std::string& Output::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Returns the password to use when connecting to DB.
 */
const std::string& Output::GetPassword() const throw ()
{
  return (this->strings_[PASSWORD]);
}

/**
 *  Returns the prefix of table names.
 */
const std::string& Output::GetPrefix() const throw ()
{
  return (this->strings_[PREFIX]);
}

/**
 *  Returns the type of the input (file or DB).
 */
const std::string& Output::GetType() const throw ()
{
  return (this->strings_[TYPE]);
}

/**
 *  Returns the user name to use when connecting to DB.
 */
const std::string& Output::GetUser() const throw ()
{
  return (this->strings_[USER]);
}

/**
 *  Set the DB name.
 */
void Output::SetDb(const std::string& db)
{
  this->strings_[DB] = db;
  return ;
}

/**
 *  Set the host name.
 */
void Output::SetHost(const std::string& host)
{
  this->strings_[HOST] = host;
  return ;
}

/**
 *  Set the password to use when connecting to DB.
 */
void Output::SetPassword(const std::string& password)
{
  this->strings_[PASSWORD] = password;
  return ;
}

/**
 *  Set the table prefix.
 */
void Output::SetPrefix(const std::string& prefix)
{
  this->strings_[PREFIX] = prefix;
  return ;
}

/**
 *  Set the type of the output (file or db).
 */
void Output::SetType(const std::string& type)
{
  this->strings_[TYPE] = type;
  return ;
}

/**
 *  Set the user name to use when connecting to the DB.
 */
void Output::SetUser(const std::string& user)
{
  this->strings_[USER] = user;
  return ;
}
