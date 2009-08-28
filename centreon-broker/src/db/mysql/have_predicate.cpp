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
#include <sstream>
#include "db/mysql/have_predicate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Expand a AND predicate.
 *
 *  Expand the AND predicate to its corresponding string.
 *
 *  \param[in] a_n_d Predicate to expand.
 */
void MySQLHavePredicate::Visit(const And& a_n_d)
{
  a_n_d.Left().Accept(*this);
  this->query_->append(" AND ");
  a_n_d.Right().Accept(*this);
  return ;
}

/**
 *  \brief Expand a Equal predicate.
 *
 *  Expand the Equal predicate to its corresponding string.
 *
 *  \param[in] equal Predicate to expand.
 */
void MySQLHavePredicate::Visit(const Equal& equal)
{
  equal.Left().Accept(*this);
  this->query_->append("=");
  equal.Right().Accept(*this);
  return ;
}

/**
 *  \brief Expand a Field predicate.
 *
 *  Append the field name to the query string.
 *
 *  \param[in] field Field to append.
 */
void MySQLHavePredicate::Visit(const Field& field)
{
  this->query_->append(field.Name());
  return ;
}

/**
 *  \brief Add a placeholder in the predicate.
 *
 *  When exploring a predicate and a placeholder is found, a ? is appened to
 *  the predicate string. This is an argument that will have to be set later
 *  before query execution.
 *
 *  \param[in] placeholder Specify that the query contains an undefined
 *                         argument.
 */
void MySQLHavePredicate::Visit(const Placeholder& placeholder)
{
  (void)placeholder;
  this->query_->append("?");
  this->placeholders++;
  return ;
}

/**
 *  \brief Expand a terminal value.
 *
 *  Append the terminal value to the query string.
 *
 *  \param[in] terminal Terminal to append.
 */
void MySQLHavePredicate::Visit(const Terminal& terminal)
{
  std::stringstream ss;

  switch (terminal.GetType())
    {
     case 'b':
      ss << terminal.GetBool();
      break ;
     case 'd':
      ss << terminal.GetDouble();
      break ;
     case 'i':
      ss << terminal.GetInt();
      break ;
     case 's':
      ss << terminal.GetShort();
      break ;
     case 'S':
      ss << '"' << terminal.GetString() << '"';
      break ;
     case 't':
      ss << terminal.GetTime();
      break ;
     default:
      assert(false);
    }
  this->query_->append(ss.str());
  return ;
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief MySQLHavePredicate default constructor.
 *
 *  Initialize members to their default value.
 */
MySQLHavePredicate::MySQLHavePredicate() : placeholders(0) {}

/**
 *  \brief MySQLHavePredicate copy constructor.
 *
 *  Build the new object by copying data from the given object.
 *
 *  \param[in] myhp Object to copy data from.
 */
MySQLHavePredicate::MySQLHavePredicate(const MySQLHavePredicate& myhp)
  : HaveArgs(myhp), HavePredicate(myhp), PredicateVisitor(myhp)
{
  this->placeholders = myhp.placeholders;
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] myhp Object to copy data from.
 *
 *  \return *this
 */
MySQLHavePredicate& MySQLHavePredicate::operator=(const MySQLHavePredicate& m)
{
  this->HavePredicate::operator=(m);
  this->PredicateVisitor::operator=(m);
  this->placeholders = m.placeholders;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief MySQLHavePredicate destructor.
 *
 *  Release previously acquired ressources.
 */
MySQLHavePredicate::~MySQLHavePredicate() {}

/**
 *  \brief Append the predicate to the query.
 *
 *  Generate the string corresponding to the predicate in a prepared statement.
 *
 *  \param[out] query Query string on which the predicate will be append.
 */
void MySQLHavePredicate::PreparePredicate(std::string& query)
{
  if (this->predicate)
    {
      this->query_ = &query;
      this->query_->append(" WHERE ");
      this->predicate->Accept(*this);
    }
  return ;
}

/**
 *  \brief Append the predicate to the query.
 *
 *  Generate the string corresponding to the predicate. The string is a plain
 *  text one.
 *
 *  \param[out] query Query string on which the predicate will be append.
 */
void MySQLHavePredicate::ProcessPredicate(std::string& query)
{
  if (this->predicate)
    this->PreparePredicate(query);
  return ;
}
