/*
** Copyright 2011-2012,2015 Merethis
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
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/processing/feeder.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
feeder::feeder() {}

/**
 *  Destructor.
 */
feeder::~feeder() {}

/**
 *  Prepare the object before running.
 *
 *  @param[in]     name  Feeding process name.
 *  @param[in]     in    Input object.
 *  @param[in,out] out   Output object.
 */
void feeder::prepare(
               std::string const& name,
               misc::shared_ptr<io::stream> in,
               misc::shared_ptr<io::stream> out) {
  _name = name;
  _in = in;
  _out = out;
  return ;
}

/**
 *  Thread main routine.
 */
void feeder::run() {
  logging::info(logging::medium)
    << "feeder: thread of '" << _name << "' is starting";
  try {
    if (_in.isNull())
      throw (exceptions::msg() << "could not process '"
             << _name << "' with no event source");
    if (_out.isNull())
      throw (exceptions::msg() << "could not process '"
             << _name << "' with no event receiver");
    while (!should_exit()) {
      misc::shared_ptr<io::data> data;
      _in->read(data);
      _out->write(data);
    }
  }
  catch (io::exceptions::shutdown const& e) {
    // Normal termination.
    (void)e;
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "feeder: error occured while processing '"
      << _name << "': " << e.what();
  }
  catch (...) {
    logging::error(logging::high)
      << "feeder: unknown error occured while processing '"
      << _name << "'";
  }
  _in.clear();
  _out.clear();
  logging::info(logging::medium)
    << "feeder: thread of '" << _name << "' will exit";
  return ;
}
