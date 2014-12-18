/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <string>
#include "com/centreon/broker/bam/configuration/bool_expression.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] id             BA id.
 *  @param[in] expression     BA expression.
 *  @param[in] impact_if      BA impact_if
 */
bool_expression::bool_expression(
                   unsigned int id,
                   std::string const& expression,
                   bool impact_if)
  : _id(id),
    _expression(expression),
    _impact_if(impact_if) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
bool_expression::bool_expression(bool_expression const& other)
  : _id(other._id),
    _expression(other._expression),
    _impact_if(other._impact_if) {}

/**
 *  Destructor
 */
bool_expression::~bool_expression() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
bool_expression& bool_expression::operator=(bool_expression const& other) {
  if (&other != this) {
    _id = other._id;
    _expression = other._expression;
    _impact_if = other._impact_if;
  }
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if this and other objects are equal.
 */
bool bool_expression::operator==(bool_expression const& other) const {
  return ((_id == other._id)
          && (_expression == other._expression)
          && (_impact_if == other._impact_if));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if this and other objects are equal.
 */
bool bool_expression::operator!=(bool_expression const& other) const {
  return (!operator==(other));
}

/**
 *  Get the primary key.
 *
 *  @return The id.
 */
unsigned int bool_expression::get_id() const {
  return (_id);
}

/**
 *  Get the boolean expression.
 *
 *  @return The textual representation of the expression.
 */
std::string const& bool_expression::get_expression() const {
  return (_expression);
}

/**
 *  Get impact if.
 *
 *  @result Get whether the impacts is applicable
 *          for a true or false statement.
 */
bool bool_expression::get_impact_if() const {
  return (_impact_if);
}

/**
 *  Set expression
 *
 *  @param[in]  exp Set the textual value for the expression.
 */
void bool_expression::set_expression(std::string const& exp) {
  _expression = exp;
}

/**
 *  Set the boolean expression ID.
 *
 *  @param[in] id  Boolean expression ID.
 */
void bool_expression::set_id(unsigned int id) {
  _id = id;
  return ;
}

/**
 *  Set impact if.
 *
 *  @param[in] bif  Sets whether the resulting value is to be considered
 *                  for a true or false evaluation.
 */
void bool_expression::set_impact_if(bool bif) {
  _impact_if = bif;
}
