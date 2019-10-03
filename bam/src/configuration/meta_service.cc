/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/configuration/meta_service.hh"

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 *
 *  @param[in] id              Meta-service ID.
 *  @param[in] name            Meta-service name.
 *  @param[in] computation     Computation function.
 *  @param[in] warning_level   Warning level.
 *  @param[in] critical_level  Critical level.
 *  @param[in] filter          Service description filter (SQL LIKE).
 *  @param[in] metric          Metric name.
 */
meta_service::meta_service(uint32_t id,
                           std::string const& name,
                           std::string const& computation,
                           double warning_level,
                           double critical_level,
                           std::string const& filter,
                           std::string const& metric)
    : _computation(computation),
      _id(id),
      _host_id(0),
      _service_id(0),
      _level_critical(critical_level),
      _level_warning(warning_level),
      _metric_name(metric),
      _name(name),
      _service_filter(filter) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
meta_service::meta_service(meta_service const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
meta_service::~meta_service() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
meta_service& meta_service::operator=(meta_service const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Equality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects are equal.
 */
bool meta_service::operator==(meta_service const& other) const {
  return ((_id == other._id) && (_host_id == other._host_id) &&
          (_service_id == other._service_id) &&
          (_level_critical == other._level_critical) &&
          (_level_warning == other._level_warning) &&
          (_computation == other._computation) &&
          (_metric_name == other._metric_name) && (_name == other._name) &&
          (_service_filter == other._service_filter) &&
          (_metrics == other._metrics));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to compare to.
 *
 *  @return True if both objects differ.
 */
bool meta_service::operator!=(meta_service const& other) const {
  return (!operator==(other));
}

/**
 *  Get computation function.
 *
 *  @return Computation function.
 */
std::string const& meta_service::get_computation() const {
  return (_computation);
}

/**
 *  Get the meta-service ID.
 *
 *  @return This meta-service ID.
 */
uint32_t meta_service::get_id() const {
  return (_id);
}

/**
 *  Get the meta-service's virtual host ID.
 *
 *  @return Virtual host ID.
 */
uint32_t meta_service::get_host_id() const {
  return (_host_id);
}

/**
 *  Get the meta-service's virtual service ID.
 *
 *  @return Virtual service ID.
 */
uint32_t meta_service::get_service_id() const {
  return (_service_id);
}

/**
 *  Get the critical level.
 *
 *  @return Critical level.
 */
double meta_service::get_level_critical() const {
  return (_level_critical);
}

/**
 *  Get the warning level.
 *
 *  @return Warning level.
 */
double meta_service::get_level_warning() const {
  return (_level_warning);
}

/**
 *  @brief Get the metric name.
 *
 *  The metric name is used to identify the metric to use to perform the
 *  computation on target services.
 *
 *  @return Metric name.
 */
std::string const& meta_service::get_metric_name() const {
  return (_metric_name);
}

/**
 *  Get the container of target metrics.
 *
 *  @return Container of metrics used to compute the meta-service value.
 */
meta_service::metric_container const& meta_service::get_metrics() const {
  return (_metrics);
}

/**
 *  Get the name of the meta-service.
 *
 *  @return Meta-service name.
 */
std::string const& meta_service::get_name() const {
  return (_name);
}

/**
 *  @brief Get service description filter.
 *
 *  This will be used in a LIKE clause of a SQL query.
 *
 *  @return Service description filter.
 */
std::string const& meta_service::get_service_filter() const {
  return (_service_filter);
}

/**
 *  Get services that impact this meta-service through their metric.
 *
 *  @return List of services that impact this meta-service.
 */
meta_service::service_container const& meta_service::get_services() const {
  return (_services);
}

/**
 *  @brief Add a target metric.
 *
 *  The metric will be used to perform the computation of this
 *  meta-service.
 *
 *  @param[in] metric_id  Metric ID.
 */
void meta_service::add_metric(uint32_t metric_id) {
  _metrics.push_back(metric_id);
  return;
}

/**
 *  Add a source service.
 *
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 */
void meta_service::add_service(uint32_t host_id, uint32_t service_id) {
  _services.insert(std::make_pair(host_id, service_id));
  return;
}

/**
 *  Set the computation function.
 *
 *  @param[in] function  Computation function. This must be one of
 *                       "SUM", "AVE", "MIN" or "MAX".
 */
void meta_service::set_computation(std::string const& function) {
  _computation = function;
  return;
}

/**
 *  Set the meta-service ID.
 *
 *  @param[in] id  Meta-service ID.
 */
void meta_service::set_id(uint32_t id) {
  _id = id;
  return;
}

/**
 *  Set the meta-service's virtual host ID.
 *
 *  @param[in] host_id  Virtual host ID.
 */
void meta_service::set_host_id(uint32_t host_id) {
  _host_id = host_id;
  return;
}

/**
 *  Set the meta-service's virtual service ID.
 *
 *  @param[in] service_id  Virtual service ID.
 */
void meta_service::set_service_id(uint32_t service_id) {
  _service_id = service_id;
  return;
}

/**
 *  Set the critical level.
 *
 *  @param[in] level  Critical level.
 */
void meta_service::set_level_critical(double level) {
  _level_critical = level;
  return;
}

/**
 *  Set the warning level.
 *
 *  @param[in] level  Warning level.
 */
void meta_service::set_level_warning(double level) {
  _level_warning = level;
  return;
}

/**
 *  Set the metric to use for the computation.
 *
 *  @param[in] metric  Metric name.
 */
void meta_service::set_metric_name(std::string const& metric) {
  _metric_name = metric;
  return;
}

/**
 *  Set the meta-service name.
 *
 *  @param[in] name  Meta-service name.
 */
void meta_service::set_name(std::string const& name) {
  _name = name;
  return;
}

/**
 *  Set service description filter.
 *
 *  @param[in] filter  Service description filter.
 */
void meta_service::set_service_filter(std::string const& filter) {
  _service_filter = filter;
  return;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void meta_service::_internal_copy(meta_service const& other) {
  _computation = other._computation;
  _id = other._id;
  _host_id = other._host_id;
  _service_id = other._service_id;
  _level_critical = other._level_critical;
  _level_warning = other._level_warning;
  _metric_name = other._metric_name;
  _metrics = other._metrics;
  _name = other._name;
  _service_filter = other._service_filter;
  return;
}
