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

#ifndef CCB_BAM_KPI_SERVICE_HH
#define CCB_BAM_KPI_SERVICE_HH

#include "com/centreon/broker/bam/kpi.hh"
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/service_listener.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class kpi_service kpi_service.hh "com/centreon/broker/bam/kpi_service.hh"
 *  @brief Service as a KPI.
 *
 *  Allows use of a service as a KPI that can impact a BA.
 */
class kpi_service : public service_listener, public kpi {
 public:
  kpi_service();
  kpi_service(kpi_service const& right);
  ~kpi_service();
  kpi_service& operator=(kpi_service const& right);
  bool child_has_update(computable* child, io::stream* visitor = NULL);
  uint32_t get_host_id() const;
  double get_impact_critical() const;
  double get_impact_unknown() const;
  double get_impact_warning() const;
  uint32_t get_service_id() const;
  short get_state_hard() const;
  short get_state_soft() const;
  short get_state_type() const;
  void impact_hard(impact_values& impact);
  void impact_soft(impact_values& impact);
  bool in_downtime() const;
  bool is_acknowledged() const;
  void service_update(std::shared_ptr<neb::service_status> const& status,
                      io::stream* visitor = NULL);
  void service_update(std::shared_ptr<neb::acknowledgement> const& ack,
                      io::stream* visitor = NULL);
  void service_update(std::shared_ptr<neb::downtime> const& dt,
                      io::stream* visitor = NULL);
  void set_acknowledged(bool acknowledged);
  void set_downtimed(bool downtimed);
  void set_host_id(uint32_t host_id);
  void set_impact_critical(double impact);
  void set_impact_unknown(double impact);
  void set_impact_warning(double impact);
  void set_service_id(uint32_t service_id);
  void set_state_hard(short state);
  void set_state_soft(short state);
  void set_state_type(short type);
  void visit(io::stream* visitor);
  virtual void set_initial_event(kpi_event const& e);
  bool ok_state() const;

 private:
  void _fill_impact(impact_values& impact, short state);
  void _internal_copy(kpi_service const& right);
  void _open_new_event(io::stream* visitor, impact_values const& impacts);

  bool _acknowledged;
  bool _downtimed;
  uint32_t _host_id;
  double _impacts[5];
  timestamp _last_check;
  std::string _output;
  std::string _perfdata;
  uint32_t _service_id;
  short _state_hard;
  short _state_soft;
  short _state_type;
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_KPI_SERVICE_HH
