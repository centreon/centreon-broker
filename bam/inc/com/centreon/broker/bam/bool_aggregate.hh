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

#ifndef CCB_BAM_BOOL_AGGREGATE_HH
#define CCB_BAM_BOOL_AGGREGATE_HH

#include <memory>
#include <string>
#include <vector>
#include "com/centreon/broker/bam/bool_metric.hh"
#include "com/centreon/broker/bam/bool_value.hh"
#include "com/centreon/broker/bam/metric_listener.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class bool_aggregate bool_aggregate.hh
 * "com/centreon/broker/bam/bool_aggregate.hh"
 *  @brief Evaluation of an aggregate of several metric.
 */
class bool_aggregate : public bool_value {
 public:
  typedef std::shared_ptr<bool_aggregate> ptr;

  static double min(std::vector<bool_metric::ptr> const& metrics);
  static double max(std::vector<bool_metric::ptr> const& metrics);
  static double avg(std::vector<bool_metric::ptr> const& metrics);
  static double sum(std::vector<bool_metric::ptr> const& metrics);
  static double count(std::vector<bool_metric::ptr> const& metrics);

  bool_aggregate(
      double (*aggregate_function)(std::vector<bool_metric::ptr> const&));
  bool_aggregate(bool_aggregate const& right);
  ~bool_aggregate();
  bool_aggregate& operator=(bool_aggregate const& right);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  double value_hard();
  double value_soft();
  bool state_known() const;

  void add_boolean_metric(bool_metric::ptr metric);
  std::vector<bool_metric::ptr> const& get_boolean_metrics() const;

 private:
  double (*_aggregate_function)(std::vector<bool_metric::ptr> const&);
  std::vector<bool_metric::ptr> _bool_metrics;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BOOL_AGGREGATE_HH
