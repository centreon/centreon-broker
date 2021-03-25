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

#ifndef CCB_BAM_CONFIGURATION_APPLIER_STATE_HH
#define CCB_BAM_CONFIGURATION_APPLIER_STATE_HH

#include <set>
#include <string>
#include "com/centreon/broker/bam/configuration/applier/ba.hh"
#include "com/centreon/broker/bam/configuration/applier/bool_expression.hh"
#include "com/centreon/broker/bam/configuration/applier/kpi.hh"
#include "com/centreon/broker/bam/configuration/applier/meta_service.hh"
#include "com/centreon/broker/bam/metric_book.hh"
#include "com/centreon/broker/bam/service_book.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
// Forward declaration.
class monitoring_stream;

namespace configuration {
// Forward declaration.
class state;

namespace applier {
/**
 *  @class state state.hh
 * "com/centreon/broker/bam/configuration/applier/state.hh"
 *  @brief Apply global state of the BAM engine.
 *
 *  Take the configuration of the BAM engine and apply it.
 */
class state {
 public:
  state();
  ~state();
  state(state const& other) = delete;
  state& operator=(state const& other) = delete;
  void apply(configuration::state const& my_state);
  metric_book& book_metric();
  service_book& book_service();
  void visit(io::stream* visitor);
  void save_to_cache(persistent_cache& cache);
  void load_from_cache(persistent_cache& cache);

 private:
  struct circular_check_node {
    circular_check_node();

    bool in_visit;
    bool visited;
    std::set<std::string> targets;
  };

  void _circular_check(configuration::state const& my_state);
  void _circular_check(circular_check_node& n);
  void _internal_copy(state const& other);

  ba _ba_applier;
  metric_book _book_metric;
  service_book _book_service;
  kpi _kpi_applier;
  bool_expression _bool_exp_applier;
  meta_service _meta_service_applier;
  std::unordered_map<std::string, circular_check_node> _nodes;
};
}  // namespace applier
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_APPLIER_STATE_HH
