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
 *  @param[in] impact         BA impact.
 *  @param[in] expression     BA expression.
 *  @param[in] impact_if      BA impact_if
 *  @param[in] state          BA state.
 */
bool_expression::bool_expression(
                   unsigned int id,
                   double impact,
                   std::string const& expression,
                   bool impact_if,
                   bool state)
  : _id(id),
    _impact(impact),
    _expression(expression),
    _impact_if(impact_if),
    _state(state) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
bool_expression::bool_expression(bool_expression const& right)
  : _id(right._id),
    _impact(right._impact),
    _expression(right._expression),
    _impact_if(right._impact),
    _state(right._state) {}

/**
 *  Destructor
 */
bool_expression::~bool_expression() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
bool_expression& bool_expression::operator=(bool_expression const& other) {
  if (&other != this) {
    _id = other._id;
    _impact = other._impact ;
    _expression = other._expression;
    _impact_if = other._impact_if;
    _state = other._state;
  }
  return (*this);
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
 *  Get the impact.
 *
 *  @return The impact refers to the negative effect of a business activity
 *          failure to the higher-level enclosing business activity. Depending
 *          on the boolean expression, this can be applicable whether the
 *          evaluation is false or true.
 *
 */
double bool_expression::get_impact() const {
  return (_impact);
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
 *  Get state.
 *
 *  @result Gets the current state.
 */
bool bool_expression::get_state() const {
  return (_state);
}

/**
 *  Set impact.
 *
 *  @param[in] si Impact value for the bool expression.
 */
void bool_expression::set_impact(double si) {
  _impact = si;
}

/**
 *  Set expression
 *
 *  @param[in]  exp Set the textual value for the expression.
 */
void bool_expression::set_expression(const std::string& exp) {
  _expression = exp;
}

/**
 *  Set impact if.
 *
 *  @param[in] bif  Sets whether the resulting value is to be considered
 *              for a true or false evaluation.
 */
void bool_expression::set_impact_if( bool bif ) {
  _impact_if = bif;
}

/**
 *  Set state.
 *
 *  @param[in]  st Set the current state of the expression.
 */
void bool_expression::set_state(bool st) {
  _state = st;
}
