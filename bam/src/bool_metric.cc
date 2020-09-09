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

#include "com/centreon/broker/bam/bool_metric.hh"

#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] metric_name  The name of the metric.
 */
bool_metric::bool_metric(std::string const& metric_name,
                         uint32_t host_id,
                         uint32_t service_id)
    : _metric_name(metric_name),
      _value(false),
      _host_id(host_id),
      _service_id(service_id) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_metric::bool_metric(bool_metric const& right)
    : bool_value(right),
      metric_listener(right),
      _metric_name(right._metric_name),
      _value(right._value),
      _host_id(right._host_id),
      _service_id(right._service_id),
      _resolved_metric_ids(right._resolved_metric_ids),
      _unknown_state_metrics(right._unknown_state_metrics) {}

/**
 *  Destructor.
 */
bool_metric::~bool_metric() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_metric& bool_metric::operator=(bool_metric const& right) {
  bool_value::operator=(right);
  if (this != &right) {
    _metric_name = right._metric_name;
    _value = right._value;
    _host_id = right._host_id;
    _service_id = right._service_id;
    _resolved_metric_ids = right._resolved_metric_ids;
    _unknown_state_metrics = right._unknown_state_metrics;
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
bool bool_metric::child_has_update(computable* child, io::stream* visitor) {
  (void)child;
  (void)visitor;
  return (true);
}

/**
 *  Get notified of metric update.
 *
 *  @param[in] m        The metric update.
 *  @param[in]visitor   A visitor.
 */
void bool_metric::metric_update(std::shared_ptr<storage::metric> const& m,
                                io::stream* visitor) {
  if (!_metric_matches(*m))
    return;

  if (_value != m->value) {
    _value = m->value;
    _values[m->metric_id] = m->value;
    propagate_update(visitor);
    _unknown_state_metrics.erase(m->metric_id);
  }
  (void)visitor;
}

/**
 *  Get the hard value.
 *
 *  @return Evaluation of the expression with hard values.
 */
double bool_metric::value_hard() {
  return (_value);
}

/**
 *  Get the soft value.
 *
 *  @return Evaluation of the expression with soft values.
 */
double bool_metric::value_soft() {
  return (_value);
}

/**
 *  Is the state known ?
 *
 *  @return  True if the state is known.
 */
bool bool_metric::state_known() const {
  return (_unknown_state_metrics.empty());
}

/**
 *  Get the name of the metric being watched.
 *
 *  @return  The name of the metric being watched.
 */
std::string const& bool_metric::get_name() const {
  return (_metric_name);
}

/**
 *  Get the host id being watched.
 *
 *  @return  The host id.
 */
uint32_t bool_metric::get_host_id() const {
  return (_host_id);
}

/**
 *  get the service id being watched.
 *
 *  @return  The service id.
 */
uint32_t bool_metric::get_service_id() const {
  return (_service_id);
}

/**
 *  Resolve the metrics.
 *
 *  @param[in] mappings  The mapping of every metric.
 */
void bool_metric::resolve_metrics(hst_svc_mapping const& mappings) {
  std::set<uint32_t> ids =
      mappings.get_metric_ids(_metric_name, _host_id, _service_id);
  if (ids.empty())
    logging::error(logging::high)
        << "bam: could not find metric ids for metric '" << _metric_name << "'";
  _resolved_metric_ids = ids;
  _unknown_state_metrics = ids;
}

/**
 *  Get the resolved metrics.
 *
 *  @return  Resolved metrics.
 */
std::set<uint32_t> const& bool_metric::get_resolved_metrics() const {
  return (_resolved_metric_ids);
}

/**
 *  Get all values.
 *
 *  @return  All values.
 */
std::map<uint32_t, double> const& bool_metric::values() const {
  return (_values);
}

/**
 *  Get if the ids of the metric matches ours.
 *
 *  @param[in] m  The metric.
 *
 *  @return  True if it matches.
 */
bool bool_metric::_metric_matches(storage::metric const& m) const {
  return (_resolved_metric_ids.find(m.metric_id) != _resolved_metric_ids.end());
}
