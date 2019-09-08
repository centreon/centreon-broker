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

#include "com/centreon/broker/bam/hst_svc_mapping.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
hst_svc_mapping::hst_svc_mapping() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
hst_svc_mapping::hst_svc_mapping(hst_svc_mapping const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
hst_svc_mapping::~hst_svc_mapping() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
hst_svc_mapping& hst_svc_mapping::operator=(hst_svc_mapping const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Get host ID by its name.
 *
 *  @param[in] hst  Host name.
 *
 *  @return Host ID, 0 if it was not found.
 */
unsigned int hst_svc_mapping::get_host_id(std::string const& hst) const {
  return (get_service_id(hst, "").first);
}

/**
 *  Get service ID by its name.
 *
 *  @param[in] hst  Host name.
 *  @param[in] svc  Service description.
 *
 *  @return Pair of integers with host ID and service ID, (0, 0) if it
 *          was not found.
 */
std::pair<unsigned int, unsigned int> hst_svc_mapping::get_service_id(
    std::string const& hst,
    std::string const& svc) const {
  std::map<std::pair<std::string, std::string>,
           std::pair<unsigned int, unsigned int> >::const_iterator
      it(_mapping.find(std::make_pair(hst, svc)));
  return ((it != _mapping.end()) ? it->second : std::make_pair(0u, 0u));
}

/**
 *  Set the ID of a host.
 *
 *  @param[in] hst      Host name.
 *  @param[in] host_id  Host ID.
 */
void hst_svc_mapping::set_host(std::string const& hst, unsigned int host_id) {
  set_service(hst, "", host_id, 0u, true);
  return;
}

/**
 *  Set the ID of a service.
 *
 *  @param[in] hst         Host name.
 *  @param[in] svc         Service description.
 *  @param[in] host_id     Host ID.
 *  @param[in] service_id  Service ID.
 */
void hst_svc_mapping::set_service(std::string const& hst,
                                  std::string const& svc,
                                  unsigned int host_id,
                                  unsigned int service_id,
                                  bool activated) {
  _mapping[std::make_pair(hst, svc)] = std::make_pair(host_id, service_id);
  _activated_mapping[std::make_pair(host_id, service_id)] = activated;
  return;
}

/**
 *  Get if the service is activated.
 *
 *  @param[in] hst_id       The host id.
 *  @param[in] service_id   The service id.
 *
 *  @return                 True if activated.
 */
bool hst_svc_mapping::get_activated(unsigned int hst_id,
                                    unsigned int service_id) const {
  std::map<std::pair<unsigned int, unsigned int>, bool>::const_iterator it(
      _activated_mapping.find(std::make_pair(hst_id, service_id)));
  return (it == _activated_mapping.end() ? true : it->second);
}

/**
 *  Register a metric.
 *
 *  @param[in] metric_id    The id of the metric.
 *  @param[in] metric_name  The name of the metric.
 *  @param[in] host_id      The id of the host.
 *  @param[in] service_id   The id of the service.
 */
void hst_svc_mapping::register_metric(unsigned int metric_id,
                                      std::string const& metric_name,
                                      unsigned int host_id,
                                      unsigned int service_id) {
  _metrics[std::make_pair(host_id, service_id)][metric_name] = metric_id;
  _metric_by_name.insert(std::make_pair(metric_name, metric_id));
}

/**
 *  Get metric ids from name/host and service ids.
 *
 *  If both host id and service id are equal to zero,
 *  will match all metric ids with the same name.
 *
 *  @param[in] metric_name   The metric name.
 *  @param[in] host_id       The host id. Can be zero.
 *  @param[in] service_id    The service id. Can be zero.
 *
 *  @return  A list of found metric ids.
 */
std::set<unsigned int> hst_svc_mapping::get_metric_ids(
    std::string const& metric_name,
    unsigned int host_id,
    unsigned int service_id) const {
  std::set<unsigned int> retval;

  if (host_id != 0 || service_id != 0) {
    std::map<std::pair<unsigned int, unsigned int>,
             std::map<std::string, unsigned int> >::const_iterator
        metrics_found = _metrics.find(std::make_pair(host_id, service_id));
    if (metrics_found == _metrics.end())
      return (retval);

    std::map<std::string, unsigned int>::const_iterator metric_found =
        metrics_found->second.find(metric_name);

    if (metric_found != metrics_found->second.end())
      retval.insert(metric_found->second);
  } else {
    std::pair<std::multimap<std::string, unsigned int>::const_iterator,
              std::multimap<std::string, unsigned int>::const_iterator>
        found = _metric_by_name.equal_range(metric_name);
    for (; found.first != found.second; ++found.first)
      retval.insert(found.first->second);
  }

  return (retval);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void hst_svc_mapping::_internal_copy(hst_svc_mapping const& other) {
  _mapping = other._mapping;
  _activated_mapping = other._activated_mapping;
  _metrics = other._metrics;
  _metric_by_name = other._metric_by_name;
  return;
}
