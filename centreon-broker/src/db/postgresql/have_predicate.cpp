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
#include "db/postgresql/have_predicate.h"

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
void PgSQLHavePredicate::Visit(const And& a_n_d)
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
void PgSQLHavePredicate::Visit(const Equal& equal)
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
void PgSQLHavePredicate::Visit(const Field& field)
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
void PgSQLHavePredicate::Visit(const Placeholder& placeholder)
{
  std::stringstream ss;

  (void)placeholder;
  ss << "$" << (this->current_arg_ + ++this->placeholders);
  this->query_->append(ss.str());
  return ;
}

/**
 *  \brief Expand a terminal value.
 *
 *  Append the terminal value to the query string.
 *
 *  \param[in] terminal Terminal to append.
 */
void PgSQLHavePredicate::Visit(const Terminal& terminal)
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
      ss << '\'' << terminal.GetString() << '\'';
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
 *  \brief PgSQLHavePredicate default constructor.
 *
 *  Initialize members to their default value.
 */
PgSQLHavePredicate::PgSQLHavePredicate() : placeholders(0) {}

/**
 *  \brief PgSQLHavePredicate copy constructor.
 *
 *  Build the new object by copying data from the given object.
 *
 *  \param[in] pghp Object to copy data from.
 */
PgSQLHavePredicate::PgSQLHavePredicate(const PgSQLHavePredicate& pghp)
  : HaveArgs(pghp), HavePredicate(pghp), PredicateVisitor(pghp)
{
  this->placeholders = pghp.placeholders;
}

/**
 *  \brief PgSQLHavePredicate destructor.
 *
 *  Release previously acquired ressources.
 */
PgSQLHavePredicate::~PgSQLHavePredicate() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] pghp Object to copy data from.
 *
 *  \return *this
 */
PgSQLHavePredicate& PgSQLHavePredicate::operator=(const PgSQLHavePredicate& p)
{
  this->HavePredicate::operator=(p);
  this->PredicateVisitor::operator=(p);
  this->placeholders = p.placeholders;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Append the predicate to the query.
 *
 *  Generate the string corresponding to the predicate in a prepared statement.
 *
 *  \param[out] query Query string on which the predicate will be append.
 */
void PgSQLHavePredicate::PreparePredicate(std::string& query,
                                          unsigned int current_arg)
{
  if (this->predicate)
    {
      this->current_arg_ = current_arg;
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
void PgSQLHavePredicate::ProcessPredicate(std::string& query)
{
  if (this->predicate)
    this->PreparePredicate(query, 0);
  return ;
}
