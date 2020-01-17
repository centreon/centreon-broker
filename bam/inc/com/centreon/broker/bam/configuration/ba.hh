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

#ifndef CCB_BAM_CONFIGURATION_BA_HH
#define CCB_BAM_CONFIGURATION_BA_HH

#include <string>
#include <vector>

#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
namespace configuration {
/**
 *  @class ba ba.hh "com/centreon/broker/bam/configuration/ba.hh"
 *  @brief BA configuration state.
 *
 *  The ba class is used to stored configuration directly read from
 *  the DB.
 */
class ba {
 public:
  typedef enum {
    state_source_impact,
    state_source_best,
    state_source_worst,
    state_source_ratio_percent,
    state_source_ratio_number
  } state_source;

  typedef enum { dt_ignore = 0, dt_inherit, dt_ignore_kpi } downtime_behaviour;

 private:
  uint32_t _id;
  uint32_t _host_id;
  uint32_t _service_id;
  std::string _name;
  ba::state_source _state_source;
  double _warning_level;
  double _critical_level;
  bam::ba_event _event;
  downtime_behaviour _dt_behaviour;

 public:
  ba(uint32_t id = 0,
     std::string const& name = "",
     ba::state_source source = state_source_impact,
     double warning_level = 0.0,
     double critical_level = 0.0,
     downtime_behaviour dt_behaviour = dt_ignore);
  ba(ba const& right);
  ~ba();
  ba& operator=(ba const& right);
  bool operator==(ba const& right) const;
  bool operator!=(ba const& right) const;

  uint32_t get_id() const;
  uint32_t get_host_id() const;
  uint32_t get_service_id() const;
  std::string const& get_name() const;
  ba::state_source get_state_source() const;
  double get_warning_level() const;
  double get_critical_level() const;
  bam::ba_event const& get_opened_event() const;
  uint32_t get_default_timeperiod() const;
  std::vector<uint32_t> const& get_timeperiods() const;
  downtime_behaviour get_downtime_behaviour() const;

  void set_id(uint32_t id);
  void set_host_id(uint32_t host_id);
  void set_service_id(uint32_t service_id);
  void set_name(std::string const& name);
  void set_state_source(ba::state_source state);
  void set_warning_level(double warning_level);
  void set_critical_level(double critical_level);
  void set_opened_event(bam::ba_event const& e);
  void set_downtime_behaviour(downtime_behaviour value);
};
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_BA_HH
