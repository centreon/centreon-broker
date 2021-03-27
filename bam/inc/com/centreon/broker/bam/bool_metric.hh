/*
** Copyright 2014, 2021 Centreon
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

#ifndef CCB_BAM_BOOL_METRIC_HH
#define CCB_BAM_BOOL_METRIC_HH

#include <map>
#include <memory>
#include <set>
#include <string>

#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/bam/hst_svc_mapping.hh"
#include "com/centreon/broker/bam/metric_listener.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_metric bool_metric.hh "com/centreon/broker/bam/bool_metric.hh"
 *  @brief Evaluation of a metric.
 *
 *  This class cache the value of a metric to compute a boolean
 *  value.
 */
class bool_metric : public bool_value, public metric_listener {
  const uint32_t _host_id;
  const uint32_t _service_id;
  const std::string _metric_name;
  double _value;

  std::set<uint32_t> _resolved_metric_ids;
  std::set<uint32_t> _unknown_state_metrics;
  std::map<uint32_t, double> _values;

  bool _metric_matches(storage::metric const& m) const;

 public:
  typedef std::shared_ptr<bool_metric> ptr;

  bool_metric(const std::string& metric_name,
              uint32_t host_id,
              uint32_t service_id);
  ~bool_metric() noexcept = default;
  bool_metric(const bool_metric&) = delete;
  bool_metric& operator=(const bool_metric&) = delete;
  bool child_has_update(computable* child,
                        io::stream* visitor = nullptr) override;
  void metric_update(const std::shared_ptr<storage::metric>& m,
                     io::stream* visitor = nullptr) override;
  double value_hard() override;
  double value_soft() override;
  bool state_known() const override;
  const std::string& get_name() const;
  uint32_t get_host_id() const;
  uint32_t get_service_id() const;
  void resolve_metrics(const hst_svc_mapping& mappings);
  const std::set<uint32_t>& get_resolved_metrics() const;
  const std::map<uint32_t, double>& values() const;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_METRIC_HH
