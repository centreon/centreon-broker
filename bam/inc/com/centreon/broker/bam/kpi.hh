/*
** Copyright 2014-2015, 2021 Centreon
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

#ifndef CCB_BAM_KPI_HH
#define CCB_BAM_KPI_HH

#include <memory>
#include <vector>

#include "com/centreon/broker/bam/computable.hh"
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
// Forward declarations.
class ba;
class impact_values;

/**
 *  @class kpi kpi.hh "com/centreon/broker/bam/kpi.hh"
 *  @brief Impact of a BA.
 *
 *  This is the base class that represents an impact of a BA. This
 *  can either be a boolean rule or a service or a BA itself.
 */
class kpi : public computable {
 protected:
  const uint32_t _id;
  const uint32_t _ba_id;
  std::shared_ptr<kpi_event> _event;
  std::vector<std::shared_ptr<kpi_event>> _initial_events;

 public:
  kpi(uint32_t kpi_id, uint32_t ba_id);
  virtual ~kpi() noexcept = default;
  kpi& operator=(const kpi&) = delete;
  kpi(const kpi&) = delete;
  uint32_t get_id() const;
  uint32_t get_ba_id() const;
  timestamp get_last_state_change() const;
  virtual void impact_hard(impact_values& hard_impact) = 0;
  virtual void impact_soft(impact_values& soft_impact) = 0;
  virtual void set_initial_event(kpi_event const& e);
  virtual void visit(io::stream* visitor) = 0;
  virtual bool in_downtime() const;
  virtual bool ok_state() const = 0;

  void commit_initial_events(io::stream* visitor);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_KPI_HH
