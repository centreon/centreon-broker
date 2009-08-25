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

#include "conf/global.h"
#include "conf/output.h"

using namespace CentreonBroker::Conf;

/**
 *  \brief Output default constructor.
 *
 *  Initialize members to default values.
 */
Output::Output()
  : connection_retry_interval_(global_conf.output.connection_retry_interval),
    query_commit_interval_(global_conf.output.query_commit_interval),
    time_commit_interval_(global_conf.output.time_commit_interval),
    type_(Output::UNKNOWN) {}

/**
 *  \brief Output copy constructor.
 *
 *  Copy parameters from the given object to the current instance.
 *
 *  \param[in] output Object to copy data from.
 */
Output::Output(const Output& output)
{
  this->operator=(output);
}

/**
 *  Output destructor.
 */
Output::~Output() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy parameters from the given object to the current instance.
 *
 *  \param[in] output Object to copy data from.
 *
 *  \return *this
 */
Output& Output::operator=(const Output& output)
{
  this->connection_retry_interval_ = output.connection_retry_interval_;
  this->db_                        = output.db_;
  this->host_                      = output.host_;
  this->name_                      = output.name_;
  this->password_                  = output.password_;
  this->query_commit_interval_     = output.query_commit_interval_;
  this->time_commit_interval_      = output.time_commit_interval_;
  this->type_                      = output.type_;
  this->user_                      = output.user_;
  return (*this);
}

/**
 *  \brief Overload of the equal operator.
 *
 *  Check if the current instance and the given object are equal. All fields
 *  are checked to declare both objects equal.
 *
 *  \return True if all parameters are equal, false otherwise.
 */
bool Output::operator==(const Output& output) const
{
  return ((this->connection_retry_interval_
           == output.connection_retry_interval_)
          && (this->db_ == output.db_)
          && (this->host_ == output.host_)
          && (this->name_ == output.name_)
          && (this->password_ == output.password_)
          && (this->query_commit_interval_ == output.query_commit_interval_)
          && (this->time_commit_interval_ == output.time_commit_interval_)
          && (this->type_ == output.type_)
          && (this->user_ == output.user_));
}

/**
 *  Overload of the not equal to operator.
 *
 *  \return The complement of the return value of operator==.
 */
bool Output::operator!=(const Output& output) const
{
  return (!(*this == output));
}

/**
 *  Overload of the less than operator.
 *
 *  \param[in] output Object to compare to.
 *
 *  \return this->GetName() < input.GetName()
 */
bool Output::operator<(const Output& output) const
{
  bool ret;

  if (this->host_ != output.host_)
    ret = (this->host_ < output.host_);
  else if (this->db_ != output.db_)
    ret = (this->db_ < output.db_);
  else
    ret = (this->user_ < output.user_);
  return (ret);
}

/**
 *  Get the amount of time in seconds that the output object should wait before
 *  attempting to reconnect to the database server.
 *
 *  \return The amount of time in seconds that the output object should wait
 *          before attempting to reconnect to the database server.
 *
 *  \see SetConnectionRetryInterval
 */
unsigned int Output::GetConnectionRetryInterval() const throw ()
{
  return (this->connection_retry_interval_);
}

/**
 *  Get the name of the DB the output object should use.
 *
 *  \return The name of the DB the output object should use.
 *
 *  \see SetDB
 */
const std::string& Output::GetDB() const throw ()
{
  return (this->db_);
}

/**
 *  Get the host name / IP address of the database server.
 *
 *  \return The host name / IP address of the database server.
 *
 *  \see SetHost
 */
const std::string& Output::GetHost() const throw ()
{
  return (this->host_);
}

/**
 *  Get the name of the output.
 *
 *  \return The name of the output.
 */
const std::string& Output::GetName() const throw ()
{
  return (this->name_);
}

/**
 *  Get the password to use when connecting to the DB server.
 *
 *  \return Password to use when connecting to the DB server.
 */
const std::string& Output::GetPassword() const throw ()
{
  return (this->password_);
}

/**
 *  Get the maximum number of queries that can be executed before a transaction
 *  commit occurs.
 *
 *  \return The maximum number of queries that can be executed before a
 *          transaction commit occurs.
 */
unsigned int Output::GetQueryCommitInterval() const throw ()
{
  return (this->query_commit_interval_);
}

/**
 *  Get the maximum amount of time in seconds that can elapse before a
 *  transaction commit occurs.
 *
 *  \return The maximum amount of time in seconds that can elapse before a
 *          transaction commit occurs.
 */
unsigned int Output::GetTimeCommitInterval() const throw ()
{
  return (this->time_commit_interval_);
}

/**
 *  \brief Get the type of the output.
 *
 *  The return value if a value of the enum Type. Currently, only databases
 *  systems are supported (MYSQL, ORACLE and POSTGRESQL).
 *
 *  \return The type of the output.
 */
Output::Type Output::GetType() const throw ()
{
  return (this->type_);
}

/**
 *  Get the user name to use when connecting to the DB server.
 *
 *  \return User name to use when connecting to the DB server.
 */
const std::string& Output::GetUser() const throw ()
{
  return (this->user_);
}

/**
 *  Set the amount of time in seconds that the output object should wait before
 *  attempting to reconnect to the database server.
 *
 *  \param[in] cri The amount of time in seconds that the output object should
 *                 wait before attempting to reconnect to the database server.
 *
 *  \see GetConnectionRetryInterval
 */
void Output::SetConnectionRetryInterval(unsigned int cri) throw ()
{
  this->connection_retry_interval_ = cri;
  return ;
}

/**
 *  Set the name of the DB the output object should use.
 *
 *  \param[in] The name of the DB the output object should use.
 *
 *  \see GetDB
 */
void Output::SetDB(const std::string& db)
{
  this->db_ = db;
  return ;
}

/**
 *  Set the host name / IP address of the database server.
 *
 *  \param[in] host The host name / IP address of the database server.
 *
 *  \see GetHost
 */
void Output::SetHost(const std::string& host)
{
  this->host_ = host;
  return ;
}

/**
 *  Set the name of the output.
 *
 *  \param[in] The name of the output.
 *
 *  \see GetName
 */
void Output::SetName(const std::string& name)
{
  this->name_ = name;
  return ;
}

/**
 *  Set the password to use when connecting to the DB server.
 *
 *  \param[in] password Password to use when connecting to the DB server.
 *
 *  \see GetPassword
 */
void Output::SetPassword(const std::string& password)
{
  this->password_ = password;
  return ;
}

/**
 *  Set the maximum number of queries that can be executed before a transaction
 *  commit occurs.
 *
 *  \param[in] qci The maximum number of queries that can be executed before a
 *                 transaction commit occurs.
 *
 *  \see GetQueryCommitInterval
 */
void Output::SetQueryCommitInterval(unsigned int qci) throw ()
{
  this->query_commit_interval_ = qci;
  return ;
}

/**
 *  Set the maximum amount of time in seconds that can elapse before a
 *  transaction commit occurs.
 *
 *  \param[in] tci The maximum amount of time in seconds that can elapse before
 *                 a transaction commit occurs.
 *
 *  \see GetTimeCommitInterval
 */
void Output::SetTimeCommitInterval(unsigned int tci) throw ()
{
  this->time_commit_interval_ = tci;
  return ;
}

/**
 *  Set the type of the output.
 *
 *  \param[in] type The type of the output.
 *
 *  \see GetType
 */
void Output::SetType(Output::Type type) throw ()
{
  this->type_ = type;
  return ;
}

/**
 *  Set the user name to use when connecting to the DB server.
 *
 *  \param[in] User name to use when connecting to the DB server.
 *
 *  \see GetUser
 */
void Output::SetUser(const std::string& user)
{
  this->user_ = user;
  return ;
}
