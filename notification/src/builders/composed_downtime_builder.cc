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

#include "com/centreon/broker/notification/builders/composed_downtime_builder.hh"

using namespace com::centreon::broker::notification;

composed_downtime_builder::composed_downtime_builder() {}

void composed_downtime_builder::add_downtime(unsigned int downtime_id,
                                             downtime::ptr downtime) {
  for (composed_builder<downtime_builder>::iterator it(begin()),
       it_end(end()); it != it_end; ++it)
    (*it)->add_downtime(downtime_id,
                        downtime);
}
