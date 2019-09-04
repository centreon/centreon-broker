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

#ifndef CCB_BAM_META_SERVICE_HH
#define CCB_BAM_META_SERVICE_HH

#include <string>
#include <unordered_map>
#include "com/centreon/broker/bam/computable.hh"
#include "com/centreon/broker/bam/metric_listener.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

// Forward declaration.
namespace storage {
class metric;
}

namespace bam {
/**
 *  @class meta_service meta_service.hh
 * "com/centreon/broker/bam/meta_service.hh"
 *  @brief Compute meta-service.
 *
 *  This is the class computing meta-services (aggregation of service
 *  metrics) with some computation method (SUM, AVERAGE, MIN, MAX,
 *  ...).
 */
class meta_service : public computable, public metric_listener {
 public:
  enum computation_type { average = 1, min, max, sum };

  meta_service();
  meta_service(meta_service const& other);
  ~meta_service();
  meta_service& operator=(meta_service const& other);
  void add_metric(unsigned int metric_id);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  unsigned int get_id() const;
  unsigned int get_host_id() const;
  unsigned int get_service_id() const;
  std::string get_output() const;
  std::string get_perfdata() const;
  short get_state() const;
  void metric_update(std::shared_ptr<storage::metric> const& m,
                     io::stream* visitor = NULL);
  void remove_metric(unsigned int metric_id);
  void recompute();
  void set_computation(computation_type type);
  void set_id(unsigned int id);
  void set_host_id(unsigned int host_id);
  void set_service_id(unsigned int service_id);
  void set_level_critical(double level);
  void set_level_warning(double level);
  void visit(io::stream* visitor, bool& changed_state);

 private:
  static int const _recompute_limit = 100;

  void _internal_copy(meta_service const& other);
  void _recompute_partial(double new_value, double old_value);
  void _send_initial_event(io::stream* visitor);
  void _send_service_status(io::stream* visitor, bool state_has_changed);

  computation_type _computation;
  unsigned int _id;
  unsigned int _host_id;
  unsigned int _service_id;
  short _last_state;
  double _level_critical;
  double _level_warning;
  std::unordered_map<unsigned int, double> _metrics;
  int _recompute_count;
  double _value;
  timestamp _last_service_status_sent;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_META_SERVICE_HH
