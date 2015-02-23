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

#include "com/centreon/broker/notification/builders/timeperiod_by_id_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 *
 *  @param[in] table  The table to fill.
 */
timeperiod_by_id_builder::timeperiod_by_id_builder(
  QHash<unsigned int, objects::timeperiod::ptr>& table)
  : _table(table) {}

/**
 *  Add a timeperiod to the builder.
 *
 *  @param[in] id   The id of the timeperiod.
 *  @param[in] con  The timeperiod to add.
 */
void timeperiod_by_id_builder::add_timeperiod(
                                 unsigned int id,
                                 objects::timeperiod::ptr con) {
  _table[id] = con;
}
