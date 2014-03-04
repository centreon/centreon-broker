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

#  include <map>
#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace  bam {
  // Forward declaration.
  class    kpi;

  /**
   *  @class ba ba.hh "com/centreon/broker/bam/ba.hh"
   *  @brief Business activity.
   *
   *  Represents a BA that gets computed every time an impact change
   *  of value.
   */
  class    ba : public computable {
  public:
           ba();
           ba(ba const& right);
           ~ba();
    ba&    operator=(ba const& right);
    void   add_impact(misc::shared_ptr<kpi>& impact);
    void   child_has_update(misc::shared_ptr<computable>& child);
    short  get_state_hard();
    short  get_state_soft();
    bool   in_downtime();
    bool   is_acknowledged();
    void   remove_impact(misc::shared_ptr<kpi>& impact);

  private:
    void   _internal_copy(ba const& right);

    bool   _acknowledged;
    bool   _downtimed;
    std::map<misc::shared_ptr<kpi>, std::pair<double, double> >
           _impacts;
    double _level_critical;
    double _level_hard;
    double _level_soft;
    double _level_warning;
  };
}

CCB_END()

#endif // !CCB_BAM_BA_HH
