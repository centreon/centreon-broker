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
#include "com/centreon/broker/exceptions/with_pointer.hh"
#include "com/centreon/broker/io/raw.hh"
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
 *  @param[in] f Object to copy.
 */
feeder::feeder(feeder const& f)
  : QThread(),
    _in(f._in),
    _out(f._out),
    _should_exit(false) {}

/**
 *  Destructor.
 */
feeder::~feeder() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
feeder& feeder::operator=(feeder const& f) {
  _in = f._in;
  _out = f._out;
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
      if (data.isNull())
        break ;
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
  catch (...) {
    if (!isRunning()) {
      _in.clear();
      _out.clear();
      throw ;
    }
  }
  _in.clear();
  _out.clear();
  return ;
}
