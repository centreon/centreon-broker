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

#ifndef CCB_BAM_KPI_BOOLEXP_HH
#  define CCB_BAM_KPI_BOOLEXP_HH

#  include "com/centreon/broker/bam/kpi.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  // Forward declaration.
  class          bool_expression;
  class          computable;

  /**
   *  @class kpi_boolexp kpi_boolexp.hh "com/centreon/broker/bam/kpi_boolexp.hh"
   *  @brief Boolean expression as a KPI.
   *
   *  This class allows you to use boolean expressions (class
   *  bool_expression) as a KPI for a BA.
   */
  class          kpi_boolexp : public kpi {
  public:
                 kpi_boolexp();
                 kpi_boolexp(kpi_boolexp const& other);
                 ~kpi_boolexp();
    kpi_boolexp& operator=(kpi_boolexp const& other);
    bool         child_has_update(
                   computable* child,
                   io::stream* visitor = NULL);
    double       get_impact() const;
    void         impact_hard(impact_values& hard_impact);
    void         impact_soft(impact_values& soft_impact);
    void         link_boolexp(
                   misc::shared_ptr<bool_expression>& my_boolexp);
    void         set_impact(double impact);
    void         unlink_boolexp();
    void         visit(io::stream* visitor);

  private:
    void         _fill_impact(impact_values& impact);
    void         _internal_copy(kpi_boolexp const& other);
    void         _open_new_event(
                   io::stream* visitor,
                   int impact,
                   short state);
    short        _get_state() const;

    misc::shared_ptr<bool_expression>
                 _boolexp;
    double       _impact;
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_BOOLEXP_HH
