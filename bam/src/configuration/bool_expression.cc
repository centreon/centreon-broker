/*
** Copyright 2009-2014 Merethis
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
#ifndef CCBN_BOOL_EXPRESSION_HH
# define CCBN_BOOL_EXPRESSION_HH

# include <string>
#include "com/centreon/broker/bam/configuration/bool_expression.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 * Constructor
 *
 *  @param[in] id             BA id.
 *  @param[in] impact         BA impact.
 *  @param[in] expression     BA expression.
 *  @param[in] impact_if      BA impact_if
 *  @param[in] state          BA state.
 *
 */
bool_expression::bool_expression(
				 unsigned int         id,
				 double               impact,
				 std::string const&   expression,
				 bool                 impact_if,
				 bool                 state
				  ):
  _id(id),
  _impact(impact),
  _expression(expression),
  _impact_if(impact_if),
  _state(state)
{}

/**
 *  assignment operator
 *
 *  @param[in] other
 *  @return    this
 */
bool_expression& bool_expression::operator=(bool_expression const& other) {
  if( &other != this){
    _id=other._id;
    _impact=other._impact ;
    _expression=other._expression;
    _impact_if=other._impact_if;
    _state=other._state;
  }
  return *this;
}

/*
 *  Destructor
 */
bool_expression::~bool_expression(){}

/**
 *  get id
 *
 *  @return The id
 *
 */
unsigned int bool_expression::get_id() const {
  return (_id);
}

/**
 *  Get the impact
 *  @return The impact
 *
 */
double bool_expression::get_impact() const {
  return (_impact);
}

/**
 *  get boolean expression
 *
 *  @return The textual representation of the expression
 */
std::string const& bool_expression::get_expression() const {
  return (_expression);
}

/**
 *  get impactIf
 *
 *  @result get whether the the expression impacts
 */
bool bool_expression::get_impact_if() const {
  return (_impact_if);
}

/**
 *  get_state
 *
 *  @result Gets the current state
 */
bool bool_expression::get_state() const {
  return (_state);
}

/**
 *  set impact
 *
 *  @param[in] impact value
 */
void bool_expression::set_impact(double d) {
  _impact = d;
}

/**
 *  set expression
 *
 *  @param[in]  set the textual value for the expression
 */
void bool_expression::set_expression(const std::string& s) {
  _expression = s;
}

/**
 *  set impactIf
 *
 *  @param[in]  sets whether the resulting value is to be considered
 */
void bool_expression::set_impact_if( bool b) {
  _impact_if = b;
}

/**
 *  set state
 *
 *  @param[in]  Set the current state of the expression
 */
void bool_expression::set_state(bool s) {
  _state = s;
}



#endif //! CCBN_BOOL_EXPRESSION_HH




