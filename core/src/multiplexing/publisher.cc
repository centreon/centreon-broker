/*
** Copyright 2011-2013,2015 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

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
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
publisher::publisher(publisher const& other) : io::stream(other) {}

/**
 *  Destructor.
 */
publisher::~publisher() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& other) {
  io::stream::operator=(other);
  return (*this);
}

/**
 *  @brief Read data.
 *
 *  Reading is not available from publisher. Therefore this method will
 *  throw an exception.
 *
 *  @param[out] d         Unused.
 *  @param[in]  deadline  Timeout.
 */
bool publisher::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (io::exceptions::shutdown(true, false)
         << "cannot read from publisher");
  return (true);
}

/**
 *  @brief Write data.
 *
 *  Send data to the multiplexing engine.
 *
 *  @param[in] d Multiplexed data.
 *
 *  @return Number of elements acknowledged (1).
 */
unsigned int publisher::write(misc::shared_ptr<io::data> const& d) {
  engine::instance().publish(d);
  return (1);
}
