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

#include "com/centreon/broker/bam/bool_expression.hh"

#include <ctime>
#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker::bam;
using namespace com::centreon::broker;

/**
 *  Default constructor.
 */
bool_expression::bool_expression() : _id(0), _impact_if(true) {}

/**
 *  Destructor.
 */
bool_expression::~bool_expression() {}

/**
 *  Base boolean expression got updated.
 *
 *  @param[in]  child    Expression that got updated.
 *  @param[out] visitor  Receive events generated by this object.
 *
 *  @return True if the values of this object were modified.
 */
bool bool_expression::child_has_update(computable* child, io::stream* visitor) {
  (void)visitor;
  // It is useless to maintain a cache of expression values in this
  // class, as the bool_* classes already cache most of them.
  if (child == _expression.get()) {
    // Logging.
    log_v2::bam()->debug(
        "BAM: boolean expression {} is getting notified of child update", _id);
    logging::debug(logging::low) << "BAM: boolean expression " << _id
                                 << " is getting notified of child update";
  }
  return (true);
}

/**
 *  Get the boolean expression state.
 *
 *  @return Either OK (0) or CRITICAL (2).
 */
impact_values::state bool_expression::get_state() const {
  return ((_expression->value_hard() == _impact_if)
              ? bool_expression::state::state_critical
              : bool_expression::state::state_ok);
}

/**
 *  Get if the state is known, i.e has been computed at least once.
 *
 *  @return  True if the state is known.
 */
bool bool_expression::state_known() const {
  return (_expression->state_known());
}

/**
 *  Get if the boolean expression is in downtime.
 *
 *  @return  True if the boolean expression is in downtime.
 */
bool bool_expression::in_downtime() const {
  return (_expression->in_downtime());
}

/**
 *  Get the expression.
 *
 *  @return  The expression.
 */
std::shared_ptr<bool_value> bool_expression::get_expression() const {
  return (_expression);
}

/**
 *  Set evaluable boolean expression.
 *
 *  @param[in] expression Boolean expression.
 */
void bool_expression::set_expression(
    std::shared_ptr<bool_value> const& expression) {
  _expression = expression;
  return;
}

/**
 *  Set boolean expression ID.
 *
 *  @param[in] id  Boolean expression ID.
 */
void bool_expression::set_id(uint32_t id) {
  _id = id;
  return;
}

/**
 *  Set whether we should impact if the expression is true or false.
 *
 *  @param[in] impact_if True if impact is applied if the expression is
 *                       true. False otherwise.
 */
void bool_expression::set_impact_if(bool impact_if) {
  _impact_if = impact_if;
  return;
}
