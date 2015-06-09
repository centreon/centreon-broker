/*
** Copyright 2011,2015 Merethis
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
 *  @param[in] other  Object to copy.
 */
hooker::hooker(hooker const& other)
  : io::stream(other), _registered(false) {
  hook(other._registered);
}

/**
 *  Destructor.
 */
hooker::~hooker() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
hooker& hooker::operator=(hooker const& other) {
  if (this != &other) {
    io::stream::operator=(other);
    hook(other._registered);
  }
  return (*this);
}

/**
 *  Enable or disable hooking.
 *
 *  @param[in] should_hook  Set to true if hooker should hook.
 */
void hooker::hook(bool should_hook) {
  if (_registered && !should_hook) {
    engine::instance().unhook(*this);
    _registered = false;
  }
  else if (!_registered && should_hook) {
    engine::instance().hook(*this);
    _registered = true;
  }
  return ;
}
