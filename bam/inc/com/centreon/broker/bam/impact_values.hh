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

#ifndef CCB_BAM_IMPACT_VALUES_HH
#  define CCB_BAM_IMPACT_VALUES_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class impact_values impact_values.hh "com/centreon/broker/bam/impact_values.hh"
   *  @brief Impact values.
   *
   *  Holds together the different values of an impact: nominal impact,
   *  downtime impact, acknowledgement impact.
   */
  class            impact_values {
  public:
                   impact_values(
                     double nominal = 0.0,
                     double acknowledgement = 0.0,
                     double downtime = 0.0);
                   impact_values(impact_values const& other);
                   ~impact_values();
    impact_values& operator=(impact_values const& other);
    double         get_acknowledgement() const;
    double         get_downtime() const;
    double         get_nominal() const;
    void           set_acknowledgement(double acknowledgement);
    void           set_downtime(double downtime);
    void           set_nominal(double nominal);

  private:
    void           _internal_copy(impact_values const& other);

    double         _acknowledgement;
    double         _downtime;
    double         _nominal;
  };
}

CCB_END()

#endif // !CCB_BAM_IMPACT_VALUES_HH
