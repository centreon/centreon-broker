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

#include "com/centreon/broker/bam/kpi.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi::kpi() : _id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
kpi::kpi(kpi const& right) :
  computable(right),
  _id(right._id),
  _event(right._event) {}

/**
 *  Destructor.
 */
kpi::~kpi() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
kpi& kpi::operator=(kpi const& right) {
  if (this != &right) {
    computable::operator=(right);
    _id = right._id;
    _event = right._event;
  }
  return (*this);
}

/**
 *  Get KPI ID.
 *
 *  @return KPI ID.
 */
unsigned int kpi::get_id() const {
  return (_id);
}

/**
 *  Set KPI ID.
 *
 *  @param[in] id KPI ID.
 */
void kpi::set_id(unsigned int id) {
  _id = id;
  return ;
}

/**
 *  Set the initial event of the kpi.
 *
 *  @param[in] e  The kpi event.
 */
void kpi::set_initial_event(kpi_event const& e) {
  if (_event.isNull())
    _event = misc::shared_ptr<kpi_event>(new kpi_event(e));
}
