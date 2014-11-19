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

#include "com/centreon/broker/notification/builders/composed_notification_method_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_notification_method_builder::composed_notification_method_builder() {}

void composed_notification_method_builder::add_notification_method(
                                             unsigned int method_id,
                                             notification_method::ptr method) {
  for (composed_builder<notification_method_builder>::iterator it(begin()),
                                                               it_end(end());
       it != it_end;
       ++it)
    (*it)->add_notification_method(method_id, method);
}
