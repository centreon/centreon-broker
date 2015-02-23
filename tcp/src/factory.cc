/*
** Copyright 2011-2013 Merethis
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
#include <QString>
#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/connector.hh"
#include "com/centreon/broker/tcp/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

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
 *  @param[in] is_input  Unused.
 *  @param[in] is_output Unused.
 *
 *  @return true if the configuration has this protocol.
 */
bool factory::has_endpoint(
                config::endpoint& cfg,
                bool is_input,
                bool is_output) const {
  (void)is_input;
  (void)is_output;
  return ((cfg.type == "ip")
          || (cfg.type == "tcp")
          || (cfg.type == "ipv4")
          || (cfg.type == "ipv6"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    Unused.
 *  @param[in]  is_output   Unused.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
                         config::endpoint& cfg,
                         bool is_input,
                         bool is_output,
                         bool& is_acceptor,
                         misc::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Find host (if exist).
  QString host;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("host"));
    if (it != cfg.params.end())
      host = it.value();
  }

  // Find port (must exist).
  unsigned short port;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("port"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "TCP: no 'port' defined for " \
               "endpoint '" << cfg.name << "'");
    port = it.value().toUShort();
  }

  // Acceptor.
  std::auto_ptr<io::endpoint> endp;
  if (host.isEmpty()) {
    is_acceptor = true;
    std::auto_ptr<tcp::acceptor> a(new tcp::acceptor);
    a->listen_on(port);
    endp.reset(a.release());
  }
  // Connector.
  else {
    is_acceptor = false;
    std::auto_ptr<tcp::connector> c(new tcp::connector);
    c->connect_to(host, port);
    c->set_timeout(is_input && is_output ? 30 : -1);
    endp.reset(c.release());
  }
  return (endp.release());
}
