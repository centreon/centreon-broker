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
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
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
feeder::feeder() : _should_exit(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
feeder::feeder(feeder const& other)
  : QThread(),
    _in(other._in),
    _out(other._out),
    _should_exit(false) {}

/**
 *  Destructor.
 */
feeder::~feeder() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
feeder& feeder::operator=(feeder const& other) {
  if (this != &other) {
    _in = other._in;
    _out = other._out;
  }
  return (*this);
}

/**
 *  Request thread termination.
 */
void feeder::exit() {
  _should_exit = true;
  if (!_in.isNull())
    _in->process(false, true);
  return ;
}

/**
 *  Prepare the object before running.
 *
 *  @param[in] in  Input object.
 *  @param[in] out Output object.
 */
void feeder::prepare(
               misc::shared_ptr<io::stream> in,
               misc::shared_ptr<io::stream> out) {
  _in = in;
  _out = out;
  return ;
}

/**
 *  Thread main routine.
 */
void feeder::run() {
  _should_exit = false;
  try {
    if (_in.isNull())
      throw (exceptions::msg()
             << "feeder: could not feed with empty input");
    if (_out.isNull())
      throw (exceptions::msg()
             << "feeder: could not feed with empty output");
    while (!_should_exit) {
      misc::shared_ptr<io::data> data;
      _in->read(data);
      try {
        _out->write(data);
      }
      catch (exceptions::msg const& e) {
        try {
          throw (exceptions::with_pointer(e, data));
        }
        catch (exceptions::with_pointer const& e) {
          throw ;
        }
        catch (...) {}
        throw ;
      }
    }
  }
  catch (io::exceptions::shutdown const& e) {
    (void)e;
    if (!isRunning()) {
      _in.clear();
      _out.clear();
      throw ;
    }
  }
  catch (std::exception const& e) {
    if (!isRunning()) {
      _in.clear();
      _out.clear();
      throw ;
    }
    else
      logging::error(logging::medium)
        << "feeder: error occured while feeding: " << e.what();
  }
  catch (...) {
    if (!isRunning()) {
      _in.clear();
      _out.clear();
      throw ;
    }
    else
      logging::error(logging::high)
        << "feeder: unknown error occured";
  }
  _in.clear();
  _out.clear();
  logging::info(logging::medium)
    << "feeder: thread will exit";
  return ;
}
