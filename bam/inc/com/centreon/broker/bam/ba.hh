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

#  include "com/centreon/broker/bam/ba_event.hh"
#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/bam/impact_values.hh"
#  include "com/centreon/broker/bam/indicator_event.hh"
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
  class          ba : public computable {
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
    double       get_downtime_impact_hard();
    double       get_downtime_impact_soft();
    unsigned int get_id();
    unsigned int get_service_id() const;
    short        get_state_hard();
    short        get_state_soft();
    void         remove_impact(misc::shared_ptr<kpi> const& impact);
    void         set_id(unsigned int id);
    void         set_service_id(unsigned int service_id);
    void         set_level_critical(double level);
    void         set_level_warning(double level);
    void         visit(stream* visitor);

  private:
    static int const        _recompute_limit = 100;

    struct impact_info {
      misc::shared_ptr<kpi> kpi_ptr;
      impact_values         hard_impact;
      impact_values         soft_impact;
    };

    void         _apply_impact(impact_info& impact);
    void         _internal_copy(ba const& right);
    void         _recompute();
    void         _unapply_impact(impact_info& impact);

    double       _acknowledgement_hard;
    double       _acknowledgement_soft;
    double       _downtime_hard;
    double       _downtime_soft;
    unsigned int _id;
    unsigned int _service_id;
    umap<kpi*, impact_info>
                 _impacts;
    double       _level_critical;
    double       _level_hard;
    double       _level_soft;
    double       _level_warning;
    int          _recompute_count;

    misc::shared_ptr<ba_event>
                 _event;

    void         _open_new_event();
    void         _generate_events(kpi const& kpi_obj,
                                  stream* visitor);
  };
}

CCB_END()

#endif // !CCB_BAM_BA_HH
