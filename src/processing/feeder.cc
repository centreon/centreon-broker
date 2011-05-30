/*
** Copyright 2011 Merethis
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

#include "exceptions/basic.hh"
#include "processing/feeder.hh"

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
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
feeder::feeder(feeder const& f) : QThread(), _in(f._in), _out(f._out) {}

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
 *  Prepare the object before running.
 *
 *  @param[in] in  Input object.
 *  @param[in] out Output object.
 */
void feeder::prepare(QSharedPointer<serialization::iserial> in,
                     QSharedPointer<serialization::oserial> out) {
  _in = in;
  _out = out;
  return ;
}

/**
 *  Thread main routine.
 */
void feeder::run() {
  try {
    if (_in.isNull())
      throw (exceptions::basic() << "could not feed with empty input");
    if (_out.isNull())
      throw (exceptions::basic() << "could not feed with empty output");
    while (true) {
      QSharedPointer<events::event> e(_in->deserialize());
      if (e.isNull())
        break ;
      _out->serialize(e);
    }
  }
  catch (...) {
    if (!isRunning())
      throw ;
  }
  return ;
}
