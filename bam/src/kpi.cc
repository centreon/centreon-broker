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
kpi::kpi(kpi const& right) : computable(right), _id(right._id) {}

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
  }
  return (*this);
}

/**
 *  Add a target BA to this KPI.
 *
 *  @param[in,out] parent  Parent BA.
 */
void kpi::add_ba(misc::shared_ptr<ba> const& parent) {
  _bas.push_back(parent);
  add_parent(parent.staticCast<computable>());
  return ;
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
 *  Remove a target BA from this KPI.
 *
 *  @param[in] parent  Parent BA.
 */
void kpi::remove_ba(misc::shared_ptr<ba> const& parent) {
  for (std::list<misc::shared_ptr<ba> >::iterator
         it(_bas.begin()),
         end(_bas.end());
       it != end;
       ++it)
    if (it->data() == parent.data()) {
      _bas.erase(it);
      break ;
    }
  remove_parent(parent);
  return ;
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
 *  Check if start time makes current event an historical event.
 *
 *  @param[in] event_start_time  Event start time.
 */
bool kpi::_is_historical_event(timestamp event_start_time) {
  bool historic(false);
  for (std::list<misc::shared_ptr<ba> >::iterator
         it(_bas.begin()),
         end(_bas.end());
       it != end;
       ++it) {
    ba_event* bae((*it)->get_ba_event());
    if (bae && (event_start_time < bae->start_time))
      historic = true;
  }
  return (historic);
}
