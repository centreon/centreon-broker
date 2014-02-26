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

using namespace com::centreon::broker::configuration;

/**
 *
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
 *
 *
 */  
unsigned int bool_expression::get_id()const { 
  return _id;
}

/**
 *
 *
 */  
double bool_expression::get_impact()const { 
  return _impact;
}

/**
 *
 *
 */  
std::string const& bool_expression::get_expression()const { 
  return _expression;
}

/**
 *
 *
 */  
bool bool_expression::get_impactIf() const { 
  return _impact_if; 
}

/**
 *
 *
 */  
bool bool_expression::get_state()const { 
  return _state; 
}

/**
 *
 *
 */  
void bool_expression::set_impact(double d){  
  _impact=d;
}

/**
 *
 *
 */  
void bool_expression::set_expression(const std::string& s){  
  _expression = s;
}

/**
 *
 *
 */  
void bool_expression::set_impactIf( bool b){  
  _impact_if = b; 
}

/**
 *
 *
 */  
void bool_expression::set_state(bool s) {  
  _state = s; 
}



#endif




