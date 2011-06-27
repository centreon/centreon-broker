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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/local/acceptor.hh"
#include "com/centreon/broker/local/connector.hh"
#include "com/centreon/broker/local/factory.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::local;

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
 *  @return Copy of this factory.
 */
io::factory* factory::clone() const {
  return (new factory(*this));
}

/**
 *  Check if a configuration supports local sockets.
 *
 *  @param[in] cfg       Object configuration.
 *  @param[in] is_input  Unused.
 *  @param[in] is_output Unused.
 *
 *  @return true if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint const& cfg,
                           bool is_input,
                           bool is_output) const {
  (void)is_input;
  (void)is_output;
  return ((cfg.type == "local_client")
          || (cfg.type == "local_server")
          || (cfg.type == "unix_client")
          || (cfg.type == "unix_server"));
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[in]  is_input    Unused.
 *  @param[in]  is_output   Unused.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(config::endpoint const& cfg,
                                    bool is_input,
                                    bool is_output,
                                    bool& is_acceptor) const {
  (void)is_input;
  (void)is_output;

  // Find name.
  QString name;
  {
    QMap<QString, QString>::const_iterator it(cfg.params.find("path"));
    if (it == cfg.params.end())
      throw (exceptions::msg() << "local: factory: no 'path' defined " \
                  "for endpoint '" << cfg.name << "'");
    name = it.value();
  }

  // Connector.
  QScopedPointer<io::endpoint> endp;
  if ((cfg.type == "local_client") || (cfg.type == "unix_client")) {
    is_acceptor = false;
    QScopedPointer<local::connector> c(new local::connector);
    c->connect_to(name);
    endp.reset(c.take());
  }
  // Acceptor.
  else {
    is_acceptor = true;
    QScopedPointer<local::acceptor> a(new local::acceptor);
    a->listen_on(name);
    endp.reset(a.take());
  }

  return (endp.take());
}
