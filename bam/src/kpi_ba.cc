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
 *  Get the hard impact introduced by the BA.
 *
 *  @return Hard impact introduced by the BA.
 */
double kpi_ba::impact_hard() {
  short state(_ba->get_state_hard());
  double impact;
  if (0 == state)
    impact = 0.0;
  else if (1 == state)
    impact = _impact_warning;
  else
    impact = _impact_critical;
  return (impact);
}

/**
 *  Get the soft impact introduced by the BA.
 *
 *  @return Soft impact introduced by the BA.
 */
double kpi_ba::impact_soft() {
  short state(_ba->get_state_soft());
  double impact;
  if (0 == state)
    impact = 0.0;
  else if (1 == state)
    impact = _impact_warning;
  else
    impact = _impact_critical;
  return (impact);
}

/**
 *  Check if the BA is in downtime.
 *
 *  @return True if the BA is in downtime, false otherwise.
 */
bool kpi_ba::in_downtime() {
  // XXX
}

/**
 *  Check if the BA is acknowledged.
 *
 *  @return True if the BA is acknowledged, false otherwise.
 */
bool kpi_ba::is_acknowledged() {
  // XXX
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
 *  Unlink from BA.
 */
void kpi_ba::unlink_ba() {
  _ba.clear();
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
