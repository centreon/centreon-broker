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

#include "conf/log.h"

using namespace CentreonBroker::Conf;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Log default constructor.
 *
 *  Initialize members to default values.
 */
Log::Log() : flags_(0), type_(Log::UNKNOWN) {}

/**
 *  \brief Log copy constructor.
 *
 *  Copy all parameters of the given object to the current instance.
 *
 *  \param[in] l Object to copy data from.
 */
Log::Log(const Log& l)
{
  this->operator=(l);
}

/**
 *  Log destructor.
 */
Log::~Log() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all parameters of the given object to the current instance.
 *
 *  \param[in] l Object to copy data from.
 *
 *  \return *this
 */
Log& Log::operator=(const Log& l)
{
  this->file_path_ = l.file_path_;
  this->flags_     = l.flags_;
  this->name_      = l.name_;
  this->type_      = l.type_;
  return (*this);
}

/**
 *  \brief Overload of the equal to operator.
 *
 *  Check if the current instance and the given object are equal. To be
 *  declared equals, every parameter should be equal.
 *
 *  \return True if the two objects are equal, false otherwise.
 */
bool Log::operator==(const Log& l) const
{
  return ((this->file_path_ == l.file_path_)
          && (this->flags_ == l.flags_)
          && (this->name_ == l.name_)
          && (this->type_ == l.type_));
}

/**
 *  Overload of the not equal to operator.
 *
 *  \return The complement of the operator== return value.
 *
 *  \see operator==
 */
bool Log::operator!=(const Log& l) const
{
  return (!(*this == l));
}

/**
 *  \brief Get the path to the log file.
 *
 *  If the object is of type FILE, this method returns the path of the file
 *  were logs should be written to.
 *
 *  \return The path to the log file.
 *
 *  \see SetFilePath
 */
const std::string& Log::GetFilePath() const throw ()
{
  return (this->file_path_);
}

/**
 *  \brief Get the flags applying to the logging object.
 *
 *  These flags defines what kind of messages should be logged to the log
 *  object. Refer to the CentreonBroker::Logging class for more informations.
 *
 *  \return The flags applying to the logging object.
 *
 *  \see SetFlags
 *  \see CentreonBroker::Logging
 */
unsigned int Log::GetFlags() const throw ()
{
  return (this->flags_);
}

/**
 *  Get the name of log object.
 *
 *  \return The name of the log object.
 *
 *  \see SetName
 */
const std::string& Log::GetName() const throw ()
{
  return (this->name_);
}

/**
 *  \brief Get the type of the log object.
 *
 *  The type of a log object is a value of the Log::Type enum. Supported values
 *  are FILE (for a standalone log file), STDERR (log messages will be sent to
 *  stderr), STDOUT (log messages will be sent to stdout) and SYSLOG (log
 *  messages will be recorded in the syslog facility.
 *
 *  \see SetType
 */
Log::Type Log::GetType() const throw ()
{
  return (this->type_);
}

/**
 *  Set the path to the log file.
 *
 *  \param[in] fp The path to the log file.
 *
 *  \see GetFilePath
 */
void Log::SetFilePath(const std::string& fp)
{
  this->file_path_ = fp;
  return ;
}

/**
 *  Set the flags applying to the logging object.
 *
 *  \param[in] flags The flags applying to the logging object.
 *
 *  \see GetFlags
 */
void Log::SetFlags(unsigned int flags) throw ()
{
  this->flags_ = flags;
  return ;
}

/**
 *  Set the name of the log object.
 *
 *  \param[in] name The name of the log object.
 *
 *  \see GetName
 */
void Log::SetName(const std::string& name)
{
  this->name_ = name;
  return ;
}

/**
 *  Set the type of the log object.
 *
 *  \param[in] type Type of the log object.
 *
 *  \see GetType
 */
void Log::SetType(Log::Type type) throw ()
{
  this->type_ = type;
  return ;
}
