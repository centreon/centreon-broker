/*
** Copyright 2013 Merethis
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

#include <memory>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/node_events_factory.hh"
#include "com/centreon/broker/neb/node_events_connector.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
node_events_factory::node_events_factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
node_events_factory::node_events_factory(node_events_factory const& f)
  : io::factory(f) {}

/**
 *  Destructor.
 */
node_events_factory::~node_events_factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
node_events_factory& node_events_factory::operator=(
  node_events_factory const& f) {
  io::factory::operator=(f);
  return (*this);
}

/**
 *  Clone this object.
 *
 *  @return Exact copy of this object.
 */
io::factory* node_events_factory::clone() const {
  return (new node_events_factory(*this));
}

/**
 *  Check if a configuration match the node event layer layer.
 *
 *  @param[in] cfg       Endpoint configuration.
 *  @param[in] is_input  true if the node event layer should act as input.
 *  @param[in] is_output true if the node event layer should act as output.
 *
 *  @return true if configuration matches the node event layer.
 */
bool node_events_factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  return (cfg.type == "node_events" && is_input && !is_output);
}

/**
 *  Generate an endpoint matching a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    true if the node event layer should act as input.
 *  @param[in]  is_output   true if the node event layer should act as output.
 *  @param[out] is_acceptor Will be set to false.
 *  @param[in]  cache       Persistent cache.
 *
 *  @return Connector matching configuration.
 */
io::endpoint* node_events_factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)is_acceptor;
  (void)is_input;
  (void)is_output;

  return (new node_events_connector(cache));
}
