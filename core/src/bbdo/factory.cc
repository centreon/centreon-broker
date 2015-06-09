/*
** Copyright 2013,2015 Merethis
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
