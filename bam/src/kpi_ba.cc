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

#include "com/centreon/broker/bam/ba.hh"
#include "com/centreon/broker/bam/kpi_ba.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_ba::kpi_ba() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
kpi_ba::kpi_ba(kpi_ba const& right) : kpi(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
kpi_ba::~kpi_ba() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
kpi_ba& kpi_ba::operator=(kpi_ba const& right) {
  if (this != &right) {
    kpi::operator=(right);
    _internal_copy(right);
  }
  return (*this);
}

/**
 *  Base BA got updated.
 *
 *  @param[in] child BA that got updated.
 */
void kpi_ba::child_has_update(misc::shared_ptr<computable>& child) {
  // It is useless to maintain a cache of BA values in this class, as
  // the ba class already cache most of them.
  (void)child;
  return ;
}

/**
 *  Get the impact introduced by a CRITICAL state of the BA.
 *
 *  @return Impact if BA is CRITICAL.
 */
double kpi_ba::get_impact_critical() const {
  return (_impact_critical);
}

/**
 *  Get the impact introduced by a WARNING state of the BA.
 *
 *  @return Impact if BA is WARNING.
 */
double kpi_ba::get_impact_warning() const {
  return (_impact_warning);
}

/**
 *  Get the hard impact introduced by the BA.
 *
 *  @param[out] hard_impact Hard impacts.
 */
void kpi_ba::impact_hard(impact_values& hard_impact) {
  _fill_impact(
    hard_impact,
    _ba->get_state_hard(),
    _ba->get_ack_impact_hard(),
    _ba->get_downtime_impact_hard());
  return ;
}

/**
 *  Get the soft impact introduced by the BA.
 *
 *  @param[out] soft_impact Soft impacts.
 */
void kpi_ba::impact_soft(impact_values& soft_impact) {
  _fill_impact(
    soft_impact,
    _ba->get_state_soft(),
    _ba->get_ack_impact_soft(),
    _ba->get_downtime_impact_soft());
  return ;
}

/**
 *  Link the kpi_ba with a specific BA (class ba).
 *
 *  @param[in] my_ba Linked BA.
 */
void kpi_ba::link_ba(misc::shared_ptr<ba>& my_ba) {
  _ba = my_ba;
  return ;
}

/**
 *  Set impact if BA is CRITICAL.
 *
 *  @param[in] impact Impact if BA is CRITICAL.
 */
void kpi_ba::set_impact_critical(double impact) {
  _impact_critical = impact;
  return ;
}

/**
 *  Set impact if BA is WARNING.
 *
 *  @param[in] impact Impact if BA is WARNING.
 */
void kpi_ba::set_impact_warning(double impact) {
  _impact_warning = impact;
  return ;
}

/**
 *  Unlink from BA.
 */
void kpi_ba::unlink_ba() {
  _ba.clear();
  return ;
}

/**
 *  Fill impact_values from base values.
 *
 *  @param[out] impact          Impact values.
 *  @param[in]  state           BA state.
 *  @param[in]  acknowledgement Acknowledgement impact of the BA.
 *  @param[in]  downtime        Downtime impact of the BA.
 */
void kpi_ba::_fill_impact(
               impact_values& impact,
               short state,
               double acknowledgement,
               double downtime) {
  // Get nominal impact from state.
  double nominal;
  if (0 == state)
    nominal = 0.0;
  else if (1 == state)
    nominal = _impact_warning;
  else
    nominal = _impact_critical;
  impact.set_nominal(nominal);

  // Compute acknowledged and downtimed impacts. Acknowledgement and
  // downtime impacts provided as arguments are from the BA. Therefore
  // are used to proportionnaly compute the acknowledged and downtimed
  // impacts, relative to the nominal impact.
  if (acknowledgement < 0.0)
    acknowledgement = 0.0;
  else if (acknowledgement > 100.0)
    acknowledgement = 100.0;
  impact.set_acknowledgement(acknowledgement * nominal / 100.0);
  if (downtime < 0.0)
    downtime = 0.0;
  else if (downtime > 100.0)
    downtime = 100.0;
  impact.set_downtime(downtime * nominal / 100.0);
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void kpi_ba::_internal_copy(kpi_ba const& right) {
  _ba = right._ba;
  _impact_critical = right._impact_critical;
  _impact_warning = right._impact_warning;
  return ;
}
