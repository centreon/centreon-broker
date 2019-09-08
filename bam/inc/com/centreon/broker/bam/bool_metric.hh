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
 public:
  typedef std::shared_ptr<bool_metric> ptr;

  bool_metric(std::string const& metric_name,
              unsigned int host_id,
              unsigned int service_id);
  bool_metric(bool_metric const& right);
  ~bool_metric();
  bool_metric& operator=(bool_metric const& right);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  void metric_update(std::shared_ptr<storage::metric> const& m,
                     io::stream* visitor = NULL);
  double value_hard();
  double value_soft();
  bool state_known() const;
  std::string const& get_name() const;
  unsigned int get_host_id() const;
  unsigned int get_service_id() const;
  void resolve_metrics(hst_svc_mapping const& mappings);
  std::set<unsigned int> const& get_resolved_metrics() const;
  std::map<unsigned int, double> const& values() const;

 private:
  std::string _metric_name;
  double _value;
  unsigned int _host_id;
  unsigned int _service_id;

  std::set<unsigned int> _resolved_metric_ids;
  std::set<unsigned int> _unknown_state_metrics;
  std::map<unsigned int, double> _values;

  bool _metric_matches(storage::metric const& m) const;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_METRIC_HH
