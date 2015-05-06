/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/neb/timeperiod_linker.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::neb;
using namespace com::centreon::broker::time;

/**
 *  Default constructor.
 */
timeperiod_linker::timeperiod_linker()
  {}

/**
 *  Add a timeperiod to the builder.
 *
 *  @param[in] id   The id of the timeperiod.
 *  @param[in] con  The timeperiod to add.
 */
void timeperiod_linker::add_timeperiod(
                          unsigned int id,
                          timeperiod::ptr con) {
  _table[id] = con;
}

/**
 *  Add a timeperiod exception to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] days           The days in the exception.
 *  @param[in] timerange      The timerange of the exception.
 */
void timeperiod_linker::add_timeperiod_exception(
                          unsigned int timeperiod_id,
                          std::string const& days,
                          std::string const& timerange) {
  QHash<unsigned int, time::timeperiod::ptr>::iterator found
    = _table.find(timeperiod_id);
  if (found == _table.end())
    throw (exceptions::msg()
             << "timeperiod_linker: couldn't find timeperiod '"
             << timeperiod_id << "'' for exception");
  (*found)->add_exception(days, timerange);
}

/**
 *  Add a timeperiod exclude relation to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] exclude_id     The id of the timeperiod excluded.
 */
void timeperiod_linker::add_timeperiod_exclude_relation(
                          unsigned int timeperiod_id,
                          unsigned int exclude_id) {
  QHash<unsigned int, time::timeperiod::ptr>::iterator found
    = _table.find(timeperiod_id);
  QHash<unsigned int, time::timeperiod::ptr>::iterator excluded
    = _table.find(exclude_id);
  if (found == _table.end())
    throw (exceptions::msg()
             << "timeperiod_linker: couldn't find timeperiod '"
             << timeperiod_id << "'' for exclusion");
  if (excluded == _table.end())
    throw (exceptions::msg()
             << "timeperiod_linker: couldn't find timeperiod '"
             << exclude_id << "'' for excluded timeperiod");
  (*found)->add_excluded(*excluded);
}

/**
 *  Add a timeperiod include relation to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] include_id     The id of the timeperiod included.
 */
void timeperiod_linker::add_timeperiod_include_relation(
                          unsigned int timeperiod_id,
                          unsigned int include_id) {
  QHash<unsigned int, time::timeperiod::ptr>::iterator found
    = _table.find(timeperiod_id);
  QHash<unsigned int, time::timeperiod::ptr>::iterator included
    = _table.find(include_id);
  if (found == _table.end())
    throw (exceptions::msg()
             << "timeperiod_linker: couldn't find timeperiod '"
             << timeperiod_id << "'' for inclusion");
  if (included == _table.end())
    throw (exceptions::msg()
             << "timeperiod_linker: couldn't find timeperiod '"
             << include_id << "'' for included timeperiod");
  (*found)->add_excluded(*included);
}
