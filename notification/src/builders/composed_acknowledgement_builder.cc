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

#include "com/centreon/broker/notification/builders/composed_acknowledgement_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_acknowledgement_builder::composed_acknowledgement_builder() {}

void composed_acknowledgement_builder::add_ack(
                                         node_id id,
                                         acknowledgement::ptr ack) {
  for (composed_builder<acknowledgement_builder>::iterator it(begin()),
                                                           it_end(end());
       it != it_end;
       ++it)
    (*it)->add_ack(id, ack);
}
