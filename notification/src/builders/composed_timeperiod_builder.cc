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

#include "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_timeperiod_builder::composed_timeperiod_builder() {}

void composed_timeperiod_builder::add_timeperiod(
                                    unsigned int id,
                                    timeperiod::ptr tperiod) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->add_timeperiod(id, tperiod);
}

void composed_timeperiod_builder::add_timeperiod_exception(
                                    unsigned int timeperiod_id,
                                    std::string const& days,
                                    std::string const& timerange) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->add_timeperiod_exception(timeperiod_id, days, timerange);
}

void composed_timeperiod_builder::add_timeperiod_exclude_relation(
                                    unsigned int timeperiod_id,
                                    unsigned int exclude_id) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->add_timeperiod_exclude_relation(timeperiod_id, exclude_id);
}

void composed_timeperiod_builder::add_timeperiod_include_relation(
                                    unsigned int timeperiod_id,
                                    unsigned int include_id) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->add_timeperiod_include_relation(timeperiod_id, include_id);
}
