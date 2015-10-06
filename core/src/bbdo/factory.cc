/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/bbdo/acceptor.hh"
#include "com/centreon/broker/bbdo/connector.hh"
#include "com/centreon/broker/bbdo/factory.hh"
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/io/protocols.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

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
 *  @param[in] other  Object to copy.
 */
factory::factory(factory const& other) : io::factory(other) {}

/**
 *  Destructor.
 */
factory::~factory() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
factory& factory::operator=(factory const& other) {
  io::factory::operator=(other);
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
 *  @brief Check if a configuration supports this protocol.
 *
 *  The endpoint 'protocol' tag must have the 'bbdo' value.
 *
 *  @param[in] cfg       Object configuration.
 *
 *  @return True if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  QMap<QString, QString>::const_iterator
    it(cfg.params.find("protocol"));
  return ((it != cfg.params.end()) && (it.value() == "bbdo"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Return value.
  io::endpoint* retval(NULL);

  // Coarse endpoint ?
  bool coarse(false);
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("coarse"));
    if (it != cfg.params.end())
      coarse = config::parser::parse_boolean(*it);
  }

  // Negociation allowed ?
  bool negociate(false);
  QString extensions;
  if (!coarse) {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("negociation"));
    if ((it != cfg.params.end())
        && (*it == "no"))
      negociate = false;
    else {
      negociate = true;
      extensions = _extensions(cfg);
    }
  }

  // Ack limit.
  unsigned int ack_limit = 3000;
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("ack_limit"));
    if (it != cfg.params.end())
      ack_limit = it->toUInt();
  }

  // Create object.
  if (is_acceptor) {
    // One peer retention mode ?
    bool one_peer_retention_mode(false);
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("one_peer_retention_mode"));
    if (it != cfg.params.end())
      one_peer_retention_mode = config::parser::parse_boolean(*it);
    if (one_peer_retention_mode)
      is_acceptor = false;
    retval = new bbdo::acceptor(
                         cfg.name,
                         negociate,
                         extensions,
                         cfg.read_timeout,
                         one_peer_retention_mode,
                         coarse);
  }
  else
    retval = new bbdo::connector(
                         negociate,
                         extensions,
                         cfg.read_timeout,
                         coarse);
  return (retval);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Get available extensions for an endpoint.
 *
 *  @param[in] cfg  Endpoint configuration.
 */
QString factory::_extensions(config::endpoint& cfg) const {
  QString extensions;
  for (QMap<QString, io::protocols::protocol>::const_iterator
         it(io::protocols::instance().begin()),
         end(io::protocols::instance().end());
       it != end;
       ++it) {
    if ((it->osi_from > 1)
        && (it->osi_to < 7)
        && !it->endpntfactry->has_endpoint(cfg)
        && !it->endpntfactry->has_not_endpoint(cfg)) {
      if (!extensions.isEmpty())
        extensions.append(" ");
      extensions.append(it.key());
    }
  }
  return (extensions);
}
