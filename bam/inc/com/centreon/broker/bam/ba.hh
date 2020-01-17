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

#ifndef CCB_BAM_BA_HH
#define CCB_BAM_BA_HH

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/computable.hh"
#include "com/centreon/broker/bam/configuration/ba.hh"
#include "com/centreon/broker/bam/impact_values.hh"
#include "com/centreon/broker/bam/inherited_downtime.hh"
#include "com/centreon/broker/bam/service_listener.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace bam {
// Forward declaration.
class kpi;

/**
 *  @class ba ba.hh "com/centreon/broker/bam/ba.hh"
 *  @brief Business activity.
 *
 *  Represents a BA that gets computed every time an impact change
 *  of value.
 */
class ba : public computable, public service_listener {
 public:
  typedef impact_values::state state;

  struct impact_info {
    std::shared_ptr<kpi> kpi_ptr;
    impact_values hard_impact;
    impact_values soft_impact;
    bool in_downtime;
  };

 private:
  static int const _recompute_limit = 100;

  void _apply_impact(kpi* kpi_ptr, impact_info& impact);
  void _open_new_event(io::stream* visitor, short service_hard_state);
  void _recompute();
  void _unapply_impact(kpi* kpi_ptr, impact_info& impact);
  void _compute_inherited_downtime(io::stream* visitor);

  configuration::ba::state_source _state_source;
  ba::state _computed_soft_state;
  ba::state _computed_hard_state;
  float _num_soft_critical_childs;
  float _num_hard_critical_childs;
  double _acknowledgement_hard;
  double _acknowledgement_soft;
  double _downtime_hard;
  double _downtime_soft;
  std::shared_ptr<ba_event> _event;
  bool _generate_virtual_status;
  uint32_t _host_id;
  uint32_t _id;
  std::unordered_map<kpi*, impact_info> _impacts;
  bool _in_downtime;
  timestamp _last_kpi_update;
  double _level_critical;
  double _level_hard;
  double _level_soft;
  double _level_warning;
  std::string _name;
  int _recompute_count;
  uint32_t _service_id;
  bool _valid;
  configuration::ba::downtime_behaviour _dt_behaviour;
  std::unique_ptr<inherited_downtime> _inherited_downtime;

  void _commit_initial_events(io::stream* visitor);

  std::vector<std::shared_ptr<ba_event> > _initial_events;

 public:
  ba(bool generate_virtual_status = true);
  ba(ba const& other) = delete;
  ~ba() = default;
  ba& operator=(ba const& other) = delete;
  void add_impact(std::shared_ptr<kpi> const& impact);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  double get_ack_impact_hard();
  double get_ack_impact_soft();
  ba_event* get_ba_event();
  double get_downtime_impact_hard();
  double get_downtime_impact_soft();
  uint32_t get_id();
  uint32_t get_host_id() const;
  uint32_t get_service_id() const;
  bool get_in_downtime() const;
  timestamp get_last_kpi_update() const;
  std::string const& get_name() const;
  std::string get_output() const;
  std::string get_perfdata() const;
  ba::state get_state_hard();
  ba::state get_state_soft();
  configuration::ba::state_source get_state_source() const;
  void remove_impact(std::shared_ptr<kpi> const& impact);
  void set_id(uint32_t id);
  void set_host_id(uint32_t host_id);
  void set_service_id(uint32_t service_id);
  void set_level_critical(double level);
  void set_level_warning(double level);
  void set_initial_event(ba_event const& event);
  void set_name(std::string const& name);
  void set_valid(bool valid);
  void set_downtime_behaviour(configuration::ba::downtime_behaviour value);
  void set_state_source(configuration::ba::state_source source);
  void visit(io::stream* visitor);
  void service_update(std::shared_ptr<neb::downtime> const& dt,
                      io::stream* visitor);
  void save_inherited_downtime(persistent_cache& cache) const;
  void set_inherited_downtime(inherited_downtime const& dwn);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_BA_HH
