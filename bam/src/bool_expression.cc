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

#include "com/centreon/broker/bam/bool_expression.hh"
#include "com/centreon/broker/bam/bool_status.hh"
#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/bam/stream.hh"
#include "com/centreon/broker/bam/impact_values.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_expression::bool_expression()
  : _id(0),
    _impact_if(true),
    _impact_hard(0.0),
    _impact_soft(0.0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_expression::bool_expression(bool_expression const& right)
  : kpi(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
bool_expression::~bool_expression() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_expression& bool_expression::operator=(
                                    bool_expression const& right) {
  if (this != &right) {
    kpi::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  @brief Notify of change of a child.
 *
 *  This class does not cache child values. Therefore this method is not
 *  used within this class.
 *
 *  @param[in]  child    Unused.
 *  @param[out] visitor  Unused.
 */
void bool_expression::child_has_update(computable* child, stream* visitor) {
  (void)child;
  (void)visitor;
  return ;
}

/**
 *  Get the hard impacts.
 *
 *  @param[out] hard_impact Hard impacts.
 */
void bool_expression::impact_hard(impact_values& hard_impact) {
  bool value(_expression->value_hard());
  hard_impact.set_nominal(((value && _impact_if)
                           || (!value && !_impact_if))
                          ? _impact_hard
                          : 0.0);
  hard_impact.set_acknowledgement(0.0);
  hard_impact.set_downtime(0.0);
  return ;
}

/**
 *  Get the soft impacts.
 *
 *  @param[out] soft_impact Soft impacts.
 */
void bool_expression::impact_soft(impact_values& soft_impact) {
  bool value(_expression->value_soft());
  soft_impact.set_nominal(((value && _impact_if)
                           || (!value && !_impact_if))
                          ? _impact_soft
                          : 0.0);
  return ;
}

/**
 *  Set evaluable boolean expression.
 *
 *  @param[in] expression Boolean expression.
 */
void bool_expression::set_expression(
                        misc::shared_ptr<bool_value> const& expression) {
  _expression = expression;
  return ;
}

/**
 *  Set boolean expression ID.
 *
 *  @param[in] id  Boolean expression ID.
 */
void bool_expression::set_id(unsigned int id) {
  _id = id;
  return ;
}

/**
 *  Set hard impact.
 *
 *  @param[in] impact Hard impact.
 */
void bool_expression::set_impact_hard(double impact) {
  _impact_hard = impact;
  return ;
}

/**
 *  Set whether we should impact if the expression is true or false.
 *
 *  @param[in] impact_if True if impact is applied if the expression is
 *                       true. False otherwise.
 */
void bool_expression::set_impact_if(bool impact_if) {
  _impact_if = impact_if;
  return ;
}

/**
 *  Set soft impact.
 *
 *  @param[in] impact Soft impact.
 */
void bool_expression::set_impact_soft(double impact) {
  _impact_soft = impact;
  return ;
}

/**
 *  Visit boolean expression.
 *
 *  @param[out] visitor  Object that will receive status.
 */
void bool_expression::visit(stream* visitor) {
  misc::shared_ptr<bool_status> b(new bool_status);
  b->bool_id = _id;
  b->state = (_expression->value_hard() ? _impact_if : !_impact_if);
  visitor->write(b.staticCast<io::data>());
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void bool_expression::_internal_copy(bool_expression const& right) {
  _expression = right._expression;
  _id = right._id;
  _impact_if = right._impact_if;
  _impact_hard = right._impact_hard;
  _impact_soft = right._impact_soft;
  return ;
}
