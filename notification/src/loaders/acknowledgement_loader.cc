/*
** Copyright 2011-2013 Merethis
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

#include <utility>
#include <vector>
#include <sstream>
#include <QVariant>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/objects/acknowledgement.hh"
#include "com/centreon/broker/notification/loaders/acknowledgement_loader.hh"

using namespace com::centreon::broker::notification;

acknowledgement_loader::acknowledgement_loader() {}

void acknowledgement_loader::load(QSqlDatabase* db,
                                  acknowledgement_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

}
