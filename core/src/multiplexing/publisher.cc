/*
** Copyright 2011-2012 Merethis
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
publisher::publisher() : _process(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] p Object to copy.
 */
publisher::publisher(publisher const& p)
  : io::stream(p), _process(p._process) {}

/**
 *  Destructor.
 */
publisher::~publisher() {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Object to copy.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& p) {
  io::stream::operator=(p);
  _process = p._process;
  return (*this);
}

/**
 *  Set whether or not to process inputs and outputs.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable publisher.
 */
void publisher::process(bool in, bool out) {
  (void)in;
  _process = out;
  return ;
}

/**
 *  @brief Read data.
 *
 *  Reading is not available from publisher. Therefore this method will
 *  throw an exception.
 *
 *  @return Empty data pointer.
 */
misc::shared_ptr<io::data> publisher::read() {
  // XXX : use io::exceptions::read_error
  throw (exceptions::msg()
           << "multiplexing: attempt to read from publisher");
  return (misc::shared_ptr<io::data>());
}

/**
 *  @brief Write data.
 *
 *  Send data to the multiplexing engine.
 *
 *  @param[in] d Multiplexed data.
 */
void publisher::write(misc::shared_ptr<io::data> d) {
  if (_process)
    engine::instance().publish(d);
  else
    throw (io::exceptions::shutdown(true, true) << "publisher "
             << this << " is shutdown");
  return ;
}
