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
** <http:/www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/bam/configuration/applier/timeperiod.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::timeperiod::timeperiod() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to be copied.
 */
applier::timeperiod::timeperiod(timeperiod const& other){
  timeperiod::operator=(other);
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to be copied.
 *
 *  @return  A reference to this object.
 */
applier::timeperiod& applier::timeperiod::operator=(timeperiod const& other) {
  if (this != &other) {
    _timeperiods = other._timeperiods;
  }
  return (*this);
}

/**
 *  Apply configuration.
 *
 *  @param[in]     my_kpis Object to copy.
 */
void applier::timeperiod::apply(state::timeperiods const& my_tps) {
  // Recreate all the timeperiods from the cfg.
  _timeperiods.clear();

  for (configuration::state::timeperiods::const_iterator it(my_tps.begin()),
                                                         end(my_tps.end());
       it != end;
       ++it)
    _timeperiods[it->second.get_id()] = _create_timeperiod(it->second);
}

/**
 *  Get a timeperiod by its id.
 *
 *  @param[in] id  The id of the timeperiod.
 *
 *  @return  The timeperiod.
 */
bam::time::timeperiod::ptr
    applier::timeperiod::get_timeperiod_by_id(
        unsigned int id) {
  return (_timeperiods[id]);
}

bam::time::timeperiod::ptr applier::timeperiod::_create_timeperiod(
      configuration::timeperiod const& tp_cfg) {
  time::timeperiod::ptr tp(new time::timeperiod(tp_cfg.get_name(),
                                                tp_cfg.get_alias(),
                                                tp_cfg.get_sunday(),
                                                tp_cfg.get_monday(),
                                                tp_cfg.get_tuesday(),
                                                tp_cfg.get_wednesday(),
                                                tp_cfg.get_thursday(),
                                                tp_cfg.get_friday(),
                                                tp_cfg.get_saturday()));
  return (tp);
}
