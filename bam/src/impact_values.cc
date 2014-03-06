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

#include "com/centreon/broker/bam/impact_values.hh"

using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] nominal         Nominal impact.
 *  @param[in] acknowledgement Part of impact induced by an
 *                             acknowledgement.
 *  @param[in] downtime        Part of impact induced by a downtime.
 */
impact_values::impact_values(
                 double nominal,
                 double acknowledgement,
                 double downtime)
  : _acknowledgement(acknowledgement),
    _downtime(downtime),
    _nominal(nominal) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
impact_values::impact_values(impact_values const& right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
impact_values::~impact_values() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
impact_values& impact_values::operator=(impact_values const& right) {
  if (this != &right)
    _internal_copy(right);
  return (*this);
}

/**
 *  Get impact induced by acknowledgement.
 *
 *  @return Impact induced by some acknowledgement.
 */
double impact_values::get_acknowledgement() const {
  return (_acknowledgement);
}

/**
 *  Get impact induced by downtime.
 *
 *  @return Impact induced by some downtime.
 */
double impact_values::get_downtime() const {
  return (_downtime);
}

/**
 *  Get nominal impact.
 *
 *  @return Nominal impact.
 */
double impact_values::get_nominal() const {
  return (_nominal);
}

/**
 *  Set impact induced by acknowledgement.
 *
 *  @param[in] acknowledgement Impact induced by some acknowledgement.
 */
void impact_values::set_acknowledgement(double acknowledgement) {
  _acknowledgement = acknowledgement;
  return ;
}

/**
 *  Set impact induced by downtime.
 *
 *  @param[in] downtime Impact induced by some downtime.
 */
void impact_values::set_downtime(double downtime) {
  _downtime = downtime;
  return ;
}

/**
 *  Set nominal impact.
 *
 *  @param[in] nominal Nominal impact.
 */
void impact_values::set_nominal(double nominal) {
  _nominal = nominal;
  return ;
}
