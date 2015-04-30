/*
** Copyright 2014-2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_BAM_KPI_SERVICE_HH
#  define CCB_BAM_KPI_SERVICE_HH

#  include "com/centreon/broker/bam/kpi.hh"
#  include "com/centreon/broker/bam/kpi_event.hh"
#  include "com/centreon/broker/bam/service_listener.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class kpi_service kpi_service.hh "com/centreon/broker/bam/kpi_service.hh"
   *  @brief Service as a KPI.
   *
   *  Allows use of a service as a KPI that can impact a BA.
   */
  class          kpi_service : public service_listener, public kpi {
  public:
                 kpi_service();
                 kpi_service(kpi_service const& right);
                 ~kpi_service();
    kpi_service& operator=(kpi_service const& right);
    bool         child_has_update(
                   computable* child,
                   io::stream* visitor = NULL);
    unsigned int get_host_id() const;
    double       get_impact_critical() const;
    double       get_impact_unknown() const;
    double       get_impact_warning() const;
    unsigned int get_service_id() const;
    short        get_state_hard() const;
    short        get_state_soft() const;
    short        get_state_type() const;
    void         impact_hard(impact_values& impact);
    void         impact_soft(impact_values& impact);
    bool         in_downtime() const;
    bool         is_acknowledged() const;
    void         service_update(
                   misc::shared_ptr<neb::service_status> const& status,
                   io::stream* visitor = NULL);
    void         service_update(
                   misc::shared_ptr<neb::acknowledgement> const& ack,
                   io::stream* visitor = NULL);
    void         service_update(
                   misc::shared_ptr<neb::downtime> const& dt,
                   io::stream* visitor = NULL);
    void         set_acknowledged(bool acknowledged);
    void         set_downtimed(bool downtimed);
    void         set_host_id(unsigned int host_id);
    void         set_impact_critical(double impact);
    void         set_impact_unknown(double impact);
    void         set_impact_warning(double impact);
    void         set_service_id(unsigned int service_id);
    void         set_state_hard(short state);
    void         set_state_soft(short state);
    void         set_state_type(short type);
    void         visit(io::stream* visitor);
    virtual void set_initial_event(kpi_event const& e);

  private:
    void         _fill_impact(impact_values& impact, short state);
    void         _internal_copy(kpi_service const& right);
    void         _open_new_event(
                   io::stream* visitor,
                   impact_values const& impacts);

    bool         _acknowledged;
    bool         _downtimed;
    unsigned int _host_id;
    double       _impacts[5];
    timestamp    _last_check;
    std::string  _output;
    std::string  _perfdata;
    unsigned int _service_id;
    short        _state_hard;
    short        _state_soft;
    short        _state_type;
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_SERVICE_HH
