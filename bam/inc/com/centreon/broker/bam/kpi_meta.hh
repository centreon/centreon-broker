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

#ifndef CCB_BAM_KPI_META_HH
#define CCB_BAM_KPI_META_HH

#include <memory>
#include "com/centreon/broker/bam/kpi.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
// Forward declaration.
class meta_service;
class computable;

/**
 *  @class kpi_meta kpi_meta.hh "com/centreon/broker/bam/kpi_meta.hh"
 *  @brief Meta-service as a KPI.
 *
 *  This class allows you to use a meta-service (class meta_service)
 *  as a KPI for a BA.
 */
class kpi_meta : public kpi {
 public:
  kpi_meta();
  kpi_meta(kpi_meta const& other);
  ~kpi_meta();
  kpi_meta& operator=(kpi_meta const& other);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  double get_impact_critical() const;
  double get_impact_warning() const;
  void impact_hard(impact_values& hard_impact);
  void impact_soft(impact_values& soft_impact);
  void link_meta(std::shared_ptr<meta_service>& my_meta);
  void set_impact_critical(double impact);
  void set_impact_warning(double impact);
  void unlink_meta();
  void visit(io::stream* visitor);
  bool ok_state() const;

 private:
  void _fill_impact(impact_values& impact);
  void _internal_copy(kpi_meta const& other);
  void _open_new_event(io::stream* visitor, int impact, short state);

  std::shared_ptr<meta_service> _meta;
  double _impact_critical;
  double _impact_warning;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_KPI_META_HH
