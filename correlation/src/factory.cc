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

#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/correlation/connector.hh"
#include "com/centreon/broker/correlation/factory.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::correlation;

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
 *  Clone a factory.
 *
 *  @return A new exact copy of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if an endpoint configuration matches streams build by this
 *  factory.
 *
 *  @param[in,out] cfg  Endpoint configuration.
 *
 *  @return True if configuration matches streams build by this factory.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  bool is_correlation(!cfg.type.compare(
                                  "correlation",
                                  Qt::CaseInsensitive));
  return (is_correlation);
}

/**
 *  Create a new endpoint matching configuration.
 *
 *  @param[in,out] cfg          Endpoint configuration.
 *  @param[out]    is_acceptor  Set to false.
 *  @param[in,out] cache        Persistent cache for this endpoint.
 *
 *  @return New endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
			 bool& is_acceptor,
			 misc::shared_ptr<persistent_cache> cache) const {
  // Find correlation file.
  QString correlation_file;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("file"));
    if (it == cfg.params.end())
      throw (exceptions::msg()
	     << "correlation: no 'file' defined for endpoint '"
	     << cfg.name << "'");
    correlation_file = *it;
  }

  // Check if this is a passive endpoint.
  bool passive;
  {
    QMap<QString, QString>::const_iterator
      it(cfg.params.find("passive"));
    if (it != cfg.params.end()) {
      passive = config::parser::parse_boolean(*it);
    }
    else
      passive = false;
  }

  // Connector.
  is_acceptor = false;
  return (new correlation::connector(correlation_file, passive, cache));
}
