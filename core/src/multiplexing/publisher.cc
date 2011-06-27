/*
** Copyright 2009-2011 Merethis
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

#include <algorithm>
#include <QMutexLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/internal.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
publisher::publisher() {}

/**
 *  @brief Copy constructor.
 *
 *  As publisher does not hold any data value, this constructor is
 *  similar to the default constructor.
 *
 *  @param[in] p Unused.
 */
publisher::publisher(publisher const& p) : io::stream(p) {}

/**
 *  Destructor.
 */
publisher::~publisher() {}

/**
 *  @brief Assignment operator.
 *
 *  As publisher does not hold any data value, this assignment operator
 *  does nothing.
 *
 *  @param[in] p Unused.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& p) {
  io::stream::operator=(p);
  return (*this);
}

/**
 *  Try to read.
 *
 *  @param[in] d Unused.
 */
QSharedPointer<io::data> publisher::read() {
  throw (exceptions::msg() << "multiplexing: tried to read from " \
           "a publisher (software bug)");
  return (QSharedPointer<io::data>());
}

/**
 *  @brief Send an event to all subscribers.
 *
 *  As soon as the method returns, the event object is owned by the
 *  publisher, meaning that it'll be automatically destroyed when
 *  necessary.
 *
 *  @param[in] e Event to publish.
 */
void publisher::write(QSharedPointer<io::data> e) {
  // Send object to every subscriber.
  {
    QMutexLocker lock(&gl_subscribersm);
    for (QList<subscriber*>::iterator it = gl_subscribers.begin(),
           end = gl_subscribers.end();
         it != end;
         ++it)
      (*it)->write(e);
  }

  return ;
}
