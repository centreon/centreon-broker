/*
** Copyright 2014 Merethis
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

#ifndef CCB_BAM_BA_HH
#  define CCB_BAM_BA_HH

#  include <string>
#  include <vector>
#  include "com/centreon/broker/bam/ba_event.hh"
#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/bam/impact_values.hh"
#  include "com/centreon/broker/bam/service_listener.hh"
#  include "com/centreon/broker/bam/time/timeperiod.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/misc/unordered_hash.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  // Forward declaration.
  class          kpi;

  /**
   *  @class ba ba.hh "com/centreon/broker/bam/ba.hh"
   *  @brief Business activity.
   *
   *  Represents a BA that gets computed every time an impact change
   *  of value.
   */
  class          ba : public computable, public service_listener {
  public:
                 ba();
                 ba(ba const& right);
                 ~ba();
    ba&          operator=(ba const& right);
    void         add_impact(misc::shared_ptr<kpi> const& impact);
    bool         child_has_update(
                   computable* child,
                   stream* visitor = NULL);
    double       get_ack_impact_hard();
    double       get_ack_impact_soft();
    ba_event*    get_ba_event();
    double       get_downtime_impact_hard();
    double       get_downtime_impact_soft();
    unsigned int get_id();
    unsigned int get_host_id() const;
    unsigned int get_service_id() const;
    bool         get_in_downtime() const;
    timestamp    get_last_service_update() const;
    std::string const&
                 get_output() const;
    std::string const&
                 get_perfdata() const;
    short        get_state_hard();
    short        get_state_soft();
    void         remove_impact(misc::shared_ptr<kpi> const& impact);
    void         set_id(unsigned int id);
    void         set_host_id(unsigned int host_id);
    void         set_service_id(unsigned int service_id);
    void         set_level_critical(double level);
    void         set_level_warning(double level);
    void         set_initial_event(ba_event const& event);
    void         add_timeperiod(time::timeperiod::ptr tp);
    void         clear_timeperiods();
    void         visit(stream* visitor);
    void         service_update(
                   misc::shared_ptr<neb::service_status> const& status,
                   stream* visitor);

  private:
    static int const        _recompute_limit = 100;

    struct impact_info {
      misc::shared_ptr<kpi> kpi_ptr;
      impact_values         hard_impact;
      impact_values         soft_impact;
    };

    void         _apply_impact(impact_info& impact);
    void         _internal_copy(ba const& right);
    void         _open_new_event(stream* visitor);
    void         _recompute();
    void         _unapply_impact(impact_info& impact);

    double       _acknowledgement_hard;
    double       _acknowledgement_soft;
    double       _downtime_hard;
    double       _downtime_soft;
    misc::shared_ptr<ba_event>
                 _event;
    unsigned int _host_id;
    unsigned int _id;
    umap<kpi*, impact_info>
                 _impacts;
    bool         _in_downtime;
    timestamp    _last_service_update;
    double       _level_critical;
    double       _level_hard;
    double       _level_soft;
    double       _level_warning;
    std::string  _output;
    std::string  _perfdata;
    int          _recompute_count;
    unsigned int _service_id;
    std::vector<time::timeperiod::ptr>
                 _tps;
  };
}

CCB_END()

#endif // !CCB_BAM_BA_HH
