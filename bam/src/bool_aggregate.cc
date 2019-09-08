/*
** Copyright 2016 Centreon
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

#include "com/centreon/broker/bam/bool_aggregate.hh"

using namespace com::centreon::broker::bam;

/**
 *  Min function.
 *
 *  @param[in] metrics  The metrics.
 *
 *  @return The minimal value.
 */
double bool_aggregate::min(std::vector<bool_metric::ptr> const& metrics) {
  double retval = 0;
  if (!metrics.empty()) {
    retval = metrics[0]->value_hard();
    for (std::vector<bool_metric::ptr>::const_iterator it = metrics.begin(),
                                                       end = metrics.end();
         it != end; ++it) {
      double val = (*it)->value_hard();
      if (val < retval)
        retval = val;
    }
  }

  return (retval);
}

/**
 *  Max function.
 *
 *  @param[in] metrics  The metrics.
 *
 *  @return The maximal value.
 */
double bool_aggregate::max(std::vector<bool_metric::ptr> const& metrics) {
  double retval = 0;
  if (!metrics.empty()) {
    retval = metrics[0]->value_hard();
    for (std::vector<bool_metric::ptr>::const_iterator it = metrics.begin(),
                                                       end = metrics.end();
         it != end; ++it) {
      double val = (*it)->value_hard();
      if (val > retval)
        retval = val;
    }
  }

  return (retval);
}

/**
 *  Average function.
 *
 *  @param[in] metrics  The metrics.
 *
 *  @return  The average.
 */
double bool_aggregate::avg(std::vector<bool_metric::ptr> const& metrics) {
  double count = bool_aggregate::count(metrics);
  if (count > 0)
    return (bool_aggregate::sum(metrics) / count);
  else
    return (0);
}

/**
 *  Sum function.
 *
 *  @param[in] metrics  The metrics.
 *
 *  @return  The sum of the metrics.
 */
double bool_aggregate::sum(std::vector<bool_metric::ptr> const& metrics) {
  double retval = 0;
  for (std::vector<bool_metric::ptr>::const_iterator it = metrics.begin(),
                                                     end = metrics.end();
       it != end; ++it)
    retval += (*it)->value_hard();
  return (retval);
}

/**
 *  Count function.
 *
 *  @param[in] metrics  The number of metrics.
 *
 *  @return  The number of metrics.
 */
double bool_aggregate::count(std::vector<bool_metric::ptr> const& metrics) {
  double retval = 0;
  for (std::vector<bool_metric::ptr>::const_iterator it = metrics.begin(),
                                                     end = metrics.end();
       it != end; ++it)
    retval += (*it)->get_resolved_metrics().size();
  return (retval);
}

/**
 *  Constructor.
 *
 *  @param[in] val  The constant value to assign.
 */
bool_aggregate::bool_aggregate(
    double (*aggregate_function)(std::vector<bool_metric::ptr> const&))
    : _aggregate_function(aggregate_function) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_aggregate::bool_aggregate(bool_aggregate const& right)
    : bool_value(right) {
  _aggregate_function = right._aggregate_function;
  _bool_metrics = right._bool_metrics;
}

/**
 *  Destructor.
 */
bool_aggregate::~bool_aggregate() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_aggregate& bool_aggregate::operator=(bool_aggregate const& right) {
  bool_value::operator=(right);
  if (this != &right) {
    _aggregate_function = right._aggregate_function;
    _bool_metrics = right._bool_metrics;
  }
  return (*this);
}

/**
 *  Get notified of child update.
 *
 *  @param[in] child    The child.
 *  @param[in] visitor  A visitor.
 *
 *  @return True if the parent was modified.
 */
bool bool_aggregate::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return (true);
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
double bool_aggregate::value_hard() {
  return (_aggregate_function(_bool_metrics));
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
double bool_aggregate::value_soft() {
  return (_aggregate_function(_bool_metrics));
}

/**
 *  Is the state known ?
 *
 *  @return  True if the state is known.
 */
bool bool_aggregate::state_known() const {
  for (std::vector<bool_metric::ptr>::const_iterator it = _bool_metrics.begin(),
                                                     end = _bool_metrics.end();
       it != end; ++it)
    if (!(*it)->state_known())
      return (false);
  return (true);
}

/**
 *  Add boolean metric.
 *
 *  @param[in] metric  The boolean metric.
 */
void bool_aggregate::add_boolean_metric(bool_metric::ptr metric) {
  _bool_metrics.push_back(metric);
}

/**
 *  Get the list of boolean metrics.
 */
std::vector<bool_metric::ptr> const& bool_aggregate::get_boolean_metrics()
    const {
  return (_bool_metrics);
}
