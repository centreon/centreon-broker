/*
** Copyright 2011 Merethis
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

#include <QSharedPointer>
#include <string.h>
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;

#define MSG "0123456789abcdef"

/**
 *  Check that multiplexing subscriber works properly.
 *
 *  @return 0 on success.
 */
int main() {
  // Initialization.
  config::applier::init();
  multiplexing::engine::instance().start();

  // Subscriber.
  multiplexing::subscriber s;

  // Return value.
  int retval(0);

  // Check that subscriber is empty.
  retval |= !s.read(0).isNull();

  // Publish data.
  QSharedPointer<io::raw> data(new io::raw);
  data->append(MSG);
  multiplexing::engine::instance().publish(data.staticCast<io::data>());

  // Close subscriber.
  s.close();

  // Publish data.
  data = QSharedPointer<io::raw>(new io::raw);
  data->append(MSG);
  multiplexing::engine::instance().publish(data.staticCast<io::data>());

  // Fetch event.
  QSharedPointer<io::data> event(s.read(0));
  retval |= (event.isNull()
             || (event->type() != "com::centreon::broker::io::raw")
             || strncmp(
                  event.staticCast<io::raw>()->QByteArray::data(),
                  MSG,
                  sizeof(MSG) - 1));

  // Try reading again.
  retval |= !s.read(0).isNull();

  // Return.
  return (retval);
}
