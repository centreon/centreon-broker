/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/ndo/acceptor.hh"
#include "com/centreon/broker/ndo/connector.hh"
#include "com/centreon/broker/ndo/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
factory::factory() {}

/**
 *  Copy constructor.
 *
 *  @param[in] f Object to copy.
 */
factory::factory(factory const& f) : io::factory(f) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] f Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& f) {
  io::factory::operator=(f);
  return (*this);
}

/**
 *  Clone the factory object.
 *
 *  @return Clone of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration supports this protocol.
 *
 *  @param[in] cfg       Object configuration.
 *  @param[in] is_input  true if endpoint must act as event source.
 *  @param[in] is_output true if endpoint must act as event destination.
 *
 *  @return true if the configuration has this protocol.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  QMap<QString, QString>::const_iterator it(cfg.params.find("protocol"));
  return ((it != cfg.params.end()) && (it.value() == "ndo"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg Endpoint configuration.
 *  @param[in]  is_input    true if endpoint must act as event source.
 *  @param[in]  is_output   true if endpoint must act as event destination.
 *                          queue is full.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor) const {
  io::endpoint* retval(NULL);
  if (is_acceptor) {
    // One peer retention mode ?
    bool one_peer_retention_mode(false);
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("one_peer_retention_mode"));
    if (it != cfg.params.end())
      one_peer_retention_mode = config::parser::parse_boolean(*it);
    retval = new ndo::acceptor(
                        cfg.name,
                        is_output,
                        one_peer_retention_mode);
  }
  else
    retval = new ndo::connector(is_input, is_output);
  return (retval);
}
