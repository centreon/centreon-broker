/*
** Copyright 2015 Merethis
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
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker::multiplexing;

/**
 *  Constructor.
 *
 *  @param[in] name        Name associated to the muxer.
 *  @param[in] persistent  Whether or not the muxer is persistent.
 */
subscriber::subscriber(
              std::string const& name,
              bool persistent)
  : _muxer(new muxer(name, persistent)) {
  multiplexing::engine::instance().subscribe(_muxer.get());
}

/**
 *  Destructor.
 */
subscriber::~subscriber() {
  multiplexing::engine::instance().unsubscribe(_muxer.get());
}

/**
 *  Get muxer.
 *
 *  @return Muxer.
 */
muxer& subscriber::get_muxer() const {
  return (*_muxer);
}
