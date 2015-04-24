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

#include "com/centreon/broker/neb/node_events_connector.hh"
#include "com/centreon/broker/neb/node_events_stream.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Constructor.
 *
 *  @param[in,out] cache             Endpoint persistent cache.
 */
node_events_connector::node_events_connector(
             misc::shared_ptr<persistent_cache> cache)
  : io::endpoint(false),
    _cache(cache) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
node_events_connector::node_events_connector(node_events_connector const& other)
  : io::endpoint(other),
    _cache(other._cache) {}

/**
 *  Destructor.
 */
node_events_connector::~node_events_connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
node_events_connector& node_events_connector::operator=(
  node_events_connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _cache = other._cache;
  }
  return (*this);
}

/**
 *  Clone this connector.
 *
 *  @return A new exact copy of this connector.
 */
io::endpoint* node_events_connector::clone() const {
  return (new node_events_connector(*this));
}

/**
 *  Close.
 */
void node_events_connector::close() {
  return ;
}

/**
 *  Open a stream.
 *
 *  @return A newly opened stream.
 */
misc::shared_ptr<io::stream> node_events_connector::open() {
  return (new node_events_stream(_cache));
}

/**
 *  Open a stream.
 *
 *  @param[in] id  Unused.
 *
 *  @return A newly opened stream.
 */
misc::shared_ptr<io::stream> node_events_connector::open(QString const& id) {
  (void)id;
  return (open());
}
