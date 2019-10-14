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
  ba(unsigned int id = 0,
     std::string const& name = "",
     double warning_level = 0.0,
     double critical_level = 0.0,
     bool inherit_kpi_downtime = false);
  ba(ba const& right);
  ~ba();
  ba& operator=(ba const& right);
  bool operator==(ba const& right) const;
  bool operator!=(ba const& right) const;

  unsigned int get_id() const;
  unsigned int get_host_id() const;
  unsigned int get_service_id() const;
  std::string const& get_name() const;
  double get_warning_level() const;
  double get_critical_level() const;
  bam::ba_event const& get_opened_event() const;
  unsigned int get_default_timeperiod() const;
  std::vector<unsigned int> const& get_timeperiods() const;
  bool get_inherit_kpi_downtime() const;

  void set_id(unsigned int id);
  void set_host_id(unsigned int host_id);
  void set_service_id(unsigned int service_id);
  void set_name(std::string const& name);
  void set_warning_level(double warning_level);
  void set_critical_level(double critical_level);
  void set_opened_event(bam::ba_event const& e);
  void set_inherit_kpi_downtime(bool value);

 private:
  unsigned int _id;
  unsigned int _host_id;
  unsigned int _service_id;
  std::string _name;
  double _warning_level;
  double _critical_level;
  bam::ba_event _event;
  bool _inherit_kpi_downtime;
};
}  // namespace configuration
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_CONFIGURATION_BA_HH
