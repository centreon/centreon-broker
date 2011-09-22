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

#include "com/centreon/broker/correlation/engine.hh"

using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
engine::engine() : activated(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] e Object to copy.
 */
engine::engine(engine const& e) : io::data(e), activated(e.activated) {}

/**
 *  Destructor.
 */
engine::~engine() {}

/**
 *  Assignment operator.
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
engine& engine::operator=(engine const& e) {
  io::data::operator=(e);
  activated = e.activated;
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The string "com::centreon::broker::correlation::engine".
 */
QString const& engine::type() const {
  static QString const engine_type("com::centreon::broker::correlation::engine");
  return (engine_type);
}
