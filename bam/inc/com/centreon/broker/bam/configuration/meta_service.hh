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

#ifndef CCB_BAM_CONFIGURATION_META_SERVICE_HH
#define CCB_BAM_CONFIGURATION_META_SERVICE_HH

#include <list>
#include <set>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
namespace configuration {
/**
 *  @class meta_service meta_service.hh
 * "com/centreon/broker/bam/configuration/meta_service.hh"
 *  @brief Meta-service configuration state.
 *
 *  The meta_service class is used to store configuration read from
 *  the database.
 */
class meta_service {
 public:
  typedef std::list<uint32_t> metric_container;
  typedef std::set<std::pair<uint32_t, uint32_t> > service_container;

  meta_service(uint32_t id = 0,
               std::string const& name = "",
               std::string const& computation = "",
               double warning_level = 0.0,
               double critical_level = 0.0,
               std::string const& filter = "",
               std::string const& metric = "");
  meta_service(meta_service const& other);
  ~meta_service();
  meta_service& operator=(meta_service const& other);
  bool operator==(meta_service const& other) const;
  bool operator!=(meta_service const& other) const;

  std::string const& get_computation() const;
  uint32_t get_id() const;
  uint32_t get_host_id() const;
  uint32_t get_service_id() const;
  double get_level_critical() const;
  double get_level_warning() const;
  std::string const& get_metric_name() const;
  metric_container const& get_metrics() const;
  std::string const& get_name() const;
  std::string const& get_service_filter() const;
  service_container const& get_services() const;

  void add_metric(uint32_t metric_id);
  void add_service(uint32_t host_id, uint32_t service_id);
  void set_computation(std::string const& function);
  void set_id(uint32_t id);
  void set_host_id(uint32_t host_id);
  void set_service_id(uint32_t service_id);
  void set_level_critical(double level);
  void set_level_warning(double level);
  void set_metric_name(std::string const& metric);
  void set_name(std::string const& name);
  void set_service_filter(std::string const& filter);

 private:
  void _internal_copy(meta_service const& other);

  std::string _computation;
  uint32_t _id;
  uint32_t _host_id;
  uint32_t _service_id;
  double _level_critical;
  double _level_warning;
  std::string _metric_name;
  metric_container _metrics;
  std::string _name;
  std::string _service_filter;
  service_container _services;
};
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_META_SERVICE_HH
