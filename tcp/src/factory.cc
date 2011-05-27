/*
** Copyright 2011 Merethis
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

#include <QString>
#include "exceptions/basic.hh"
#include "tcp/acceptor.hh"
#include "tcp/connector.hh"
#include "tcp/factory.hh"

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
 *  @return true if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint const& cfg) const {
  return ((cfg.type == "ip")
          || (cfg.type == "tcp")
          || (cfg.type == "ipv4")
          || (cfg.type == "ipv6"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(config::endpoint const& cfg, bool& is_acceptor) const {
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
    if (it == cfg.params.end()) {
      throw (exceptions::basic() << "no 'port' defined for TCP endpoint '"
               << cfg.name.toStdString().c_str());
    }
    port = it.value().toUShort();
  }

  // Acceptor.
  QScopedPointer<io::endpoint> endp;
  if (host.isEmpty()) {
    is_acceptor = true;
    QScopedPointer<tcp::acceptor> a(new tcp::acceptor);
    a->listen_on(port);
    endp.reset(a.take());
  }
  // Connector.
  else {
    is_acceptor = false;
    QScopedPointer<tcp::connector> c(new tcp::connector);
    c->connect_to(host, port);
    endp.reset(c.take());
  }
  
  return (endp.take());
}
