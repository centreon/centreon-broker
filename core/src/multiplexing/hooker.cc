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

#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/hooker.hh"

using namespace com::centreon::broker::multiplexing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
hooker::hooker() : _registered(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] h Object to copy.
 */
hooker::hooker(hooker const& h) : QObject(), io::stream(h) {}

/**
 *  Destructor.
 */
hooker::~hooker() {}

/**
 *  Assignment operator.
 *
 *  @param[in] h Object to copy.
 *
 *  @return This object.
 */
hooker& hooker::operator=(hooker const& h) {
  io::stream::operator=(h);
  return (*this);
}

/**
 *  Enable or disable hooking.
 *
 *  @param[in] in  Set to false will disable hooking.
 *  @param[in] out Set to false will isable hooking.
 */
void hooker::process(bool in, bool out) {
  if (_registered && (!in || !out)) {
    engine::instance().unhook(*this);
    _registered = false;
  }
  else if (!_registered && (in || out)) {
    engine::instance().hook(*this);
    _registered = true;
  }
  return ;
}
