/*
** Copyright 2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/bam/configuration/bool_expression.hh"
#include <string>

using namespace com::centreon::broker::bam::configuration;
using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] id             BA id.
 *  @param[in] name           BA name.
 *  @param[in] expression     BA expression.
 *  @param[in] impact_if      BA impact_if
 */
bool_expression::bool_expression(unsigned int id,
                                 std::string const& name,
                                 std::string const& expression,
                                 bool impact_if)
    : _id(id), _name(name), _expression(expression), _impact_if(impact_if) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
bool_expression::bool_expression(bool_expression const& other)
    : _id(other._id),
      _name(other._name),
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
    _name = other._name;
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
  return ((_id == other._id) && (_name == other._name) &&
          (_expression == other._expression) &&
          (_impact_if == other._impact_if));
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
 *  Get the name.
 *
 *  @return The name of this expression..
 */
std::string const& bool_expression::get_name() const {
  return (_name);
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
 *  Set name.
 *
 *  @param[in]  name Set the textual value for the name.
 */
void bool_expression::set_name(std::string const& name) {
  _name = name;
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
  return;
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
